#include<string.h>
#include<assert.h>
#include "aes.h"
#include "conf.c"

#ifndef GET_UINT32
#define GET_UINT32(n,b,i) do { \
	(n) = ((uint32_t)(b)[(i)    ]      ) \
		| ((uint32_t)(b)[(i) + 1] <<  8) \
		| ((uint32_t)(b)[(i) + 2] << 16) \
		| ((uint32_t)(b)[(i) + 3] << 24);\
} while(0)
#endif

#define ROTL8(x) (((x) << 24) | ((x) >> 8))
#define ROTL16(x) (((x) << 16) | ((x) >> 16))
#define ROTL24(x) (((x) << 8) | ((x) >> 24))

#define SUB_WORD(x) (((uint32_t)S_BOX[(x)&0xFF]) \
	| ((uint32_t)S_BOX[((x) >>  8)&0xFF] << 8) \
	| ((uint32_t)S_BOX[((x) >> 16)&0xFF] << 16) \
	| ((uint32_t)S_BOX[((x) >> 24)&0xFF] << 24) \
	)
static uint8_t swap_type;

static void transport(uint8_t state[BLOCK_SIZE])
{
	assert(state != NULL);
	uint8_t _state[4][4];
	int r,c;
	for (r = 0; r < 4; ++r)
		for (c = 0; c < 4; ++c)
			_state[r][c] = state[(c<<2)+r];
	memcpy(state, _state, sizeof(_state));
}

static void add_round_key(uint8_t state[BLOCK_SIZE], const uint8_t key[BLOCK_SIZE])
{
	assert(state != NULL);
	assert(key != NULL);
	int i;
	for (i = 0; i < BLOCK_SIZE; ++i)
		state[i] ^= key[i];
}

static void _sub_bytes(uint8_t state[BLOCK_SIZE], const uint8_t *BOX)
{
	assert(state != NULL);
	assert(BOX != NULL);
	int i;
	for (i = 0; i < BLOCK_SIZE; ++i)
		state[i] = BOX[state[i]];
}

#define sub_bytes(state) _sub_bytes(state, S_BOX)
#define inv_sub_bytes(state) _sub_bytes(state, INV_S_BOX)
#define Csub_bytes(state) _sub_bytes(state, CUBE_S_BOX)
#define Cinv_sub_bytes(state) _sub_bytes(state, INV_CUBE_S_BOX)

#define _shift_rows(state, OP1, OP2, OP3) do { \
	transport(state); \
	*(uint32_t *)(state+4) = OP1(*(uint32_t *)(state+4)); \
	*(uint32_t *)(state+8) = OP2(*(uint32_t *)(state+8)); \
	*(uint32_t *)(state+12) = OP3(*(uint32_t *)(state+12)); \
	transport(state); \
} while(0)

#define shift_rows(state) _shift_rows(state, ROTL8, ROTL16, ROTL24)
#define inv_shift_rows(state) _shift_rows(state, ROTL24, ROTL16, ROTL8)

static uint8_t GF_256_multiply(uint8_t a, uint8_t b)
{
	uint8_t t[8] = { a };
	uint8_t ret = 0x00;
	int i = 0;
	for (i = 1; i < 8; ++i) {
		t[i] = t[i-1] << 1;
		if (t[i-1]&0x80) t[i] ^= 0x1b;
	}
	for (i = 0; i < 8; ++i)
		ret ^= (((b >> i) & 0x01) * t[i]);
	return ret;
}

static void _mix_columns(uint8_t state[BLOCK_SIZE], const uint8_t matrix[][4])
{
	assert(state != NULL);
	assert(matrix != NULL);
	uint8_t _state[BLOCK_SIZE] = {0};
	int r,c,i;
	for (r = 0; r < 4; ++r)
		for (c = 0; c < 4; ++c)
			for (i = 0; i < 4; ++i)
				_state[(c<<2)+r] ^= GF_256_multiply(matrix[r][i], state[(c<<2)+i]);
	memcpy(state, _state, sizeof(_state));
}

#define mix_columns(state) _mix_columns(state, MIX)
#define inv_mix_columns(state) _mix_columns(state, INV_MIX)

static void aes_round(uint8_t state[BLOCK_SIZE], const uint8_t rk[BLOCK_SIZE])
{
	sub_bytes(state);
	shift_rows(state);
	mix_columns(state);
	add_round_key(state, rk);
}
static void aes_inv_round(uint8_t state[BLOCK_SIZE], const uint8_t inv_rk[BLOCK_SIZE])
{
	inv_shift_rows(state);
	inv_sub_bytes(state);
	add_round_key(state, inv_rk);
	inv_mix_columns(state);
}

static void aes_final_round(uint8_t state[BLOCK_SIZE], const uint8_t rk[BLOCK_SIZE])
{
	sub_bytes(state);
	shift_rows(state);
	add_round_key(state, rk);
}
static void inv_final_round(uint8_t state[BLOCK_SIZE], const uint8_t inv_rk[BLOCK_SIZE])
{
	inv_shift_rows(state);
	inv_sub_bytes(state);
	add_round_key(state, inv_rk);
}

static void key_expansion(aes_context *ctx, const uint8_t *key)
{
	assert(ctx != NULL);
	assert(key != NULL);
	uint32_t Nk = ctx->nr - 6;
	uint32_t Ek = (ctx->nr+1)<<2;
	uint32_t *RK = ctx->rk;
	
	uint32_t i = 0;
	do
	{
		GET_UINT32(RK[i], key, i<<2);
	} while(++i < Nk);
	do
	{
		uint32_t t = RK[i-1];
		if ((i % Nk) == 0)
			t = SUB_WORD(ROTL8(t))^RCON[i/Nk -1];
		else if (Nk == 8 && (i % Nk) == 4)
			t = SUB_WORD(t);
		RK[i] = RK[i-Nk]^t;
	} while(++i < Ek);
}

int aes_set_key(aes_context *ctx, const uint8_t *key, uint32_t key_bit)
{
	if (ctx == NULL || key == NULL)
		return PARM_ERROR;
	switch (key_bit)
	{
		case 128: ctx->nr = 10; break;
		case 192: ctx->nr = 12; break;
		case 256: ctx->nr = 14; break;
		default: return PARM_ERROR;
	}
	ctx->rk = ctx->buf;
	key_expansion(ctx, key);
	return SUCCESS;
}

int aes_encrypt_block(aes_context *ctx
	, uint8_t cipher_text[BLOCK_SIZE], const uint8_t text[BLOCK_SIZE])
{
	if (ctx == NULL || cipher_text == NULL || text == NULL)
		return PARM_ERROR;
	if (ctx->rk != ctx->buf)
		return NOT_INIT_KEY;
	uint32_t Nr = ctx->nr;
	uint32_t *RK = ctx->rk;
	uint8_t *state = cipher_text;
	memcpy(state, text, BLOCK_SIZE);

	add_round_key(state, (const uint8_t *)RK);
	uint32_t i;
	for (i = 1; i < Nr; ++i)
		aes_round(state, (const uint8_t *)(RK + (i<<2)));
	aes_final_round(state, (const uint8_t *)(RK + (Nr<<2)));
	
	return SUCCESS;
}

int aes_decrypt_block(aes_context *ctx
	, uint8_t text[BLOCK_SIZE], const uint8_t cipher_text[BLOCK_SIZE])
{
	if (ctx == NULL || text == NULL || cipher_text == NULL)
		return PARM_ERROR;
	if (ctx->rk != ctx->buf)
		return NOT_INIT_KEY;
	uint32_t Nr = ctx->nr;
	uint32_t *INV_RK = ctx->rk;
	uint8_t *state = text;
	memcpy(state, cipher_text, BLOCK_SIZE);
	
	add_round_key(state, (const uint8_t *)(INV_RK + (Nr<<2)));
	uint32_t i;
	for (i = Nr-1; i > 0; --i)
		aes_inv_round(state, (const uint8_t *)(INV_RK + (i<<2)));
	inv_final_round(state, (const uint8_t *)INV_RK);
	
	return SUCCESS;
}




void setSWAP_Type(uint8_t type)
{
	//printf("swap_type = %d\n",swap_type);
	swap_type = type%16;
	//printf("swap_type = %d\n",swap_type);
}


void _Swap(uint8_t _state[BLOCK_SIZE], uint8_t swap_state[BLOCK_SIZE], const uint8_t type)
{
	switch (type)
	{
		case 0:
			swap_state[0] = _state[0];
			swap_state[1] = _state[1];
			swap_state[2] = _state[2];
			swap_state[3] = _state[3];
			swap_state[4] = _state[7];
			swap_state[5] = _state[11];
			swap_state[6] = _state[15];
			swap_state[7] = _state[14];
			swap_state[8] = _state[13];
			swap_state[9] = _state[12];
			swap_state[10] = _state[8];
			swap_state[11] = _state[4];
			swap_state[12] = _state[5];
			swap_state[13] = _state[6];
			swap_state[14] = _state[10];
			swap_state[15] = _state[9];
		break;
		case 1:
			swap_state[0] = _state[3];
			swap_state[1] = _state[7];
			swap_state[2] = _state[11];
			swap_state[3] = _state[15];
			swap_state[4] = _state[14];
			swap_state[5] = _state[13];
			swap_state[6] = _state[12];
			swap_state[7] = _state[8];
			swap_state[8] = _state[4];
			swap_state[9] = _state[0];
			swap_state[10] = _state[1];
			swap_state[11] = _state[2];
			swap_state[12] = _state[6];
			swap_state[13] = _state[10];
			swap_state[14] = _state[9];
			swap_state[15] = _state[5];
		break;	
		case 2:
			swap_state[0] = _state[15];
			swap_state[1] = _state[14];
			swap_state[2] = _state[13];
			swap_state[3] = _state[12];
			swap_state[4] = _state[8];
			swap_state[5] = _state[4];
			swap_state[6] = _state[0];
			swap_state[7] = _state[1];
			swap_state[8] = _state[2];
			swap_state[9] = _state[3];
			swap_state[10] = _state[7];
			swap_state[11] = _state[11];
			swap_state[12] = _state[10];
			swap_state[13] = _state[9];
			swap_state[14] = _state[5];
			swap_state[15] = _state[6];
		break;		
		case 3:
			swap_state[0] = _state[12];
			swap_state[1] = _state[8];
			swap_state[2] = _state[4];
			swap_state[3] = _state[0];
			swap_state[4] = _state[1];
			swap_state[5] = _state[2];
			swap_state[6] = _state[3];
			swap_state[7] = _state[7];
			swap_state[8] = _state[11];
			swap_state[9] = _state[15];
			swap_state[10] = _state[14];
			swap_state[11] = _state[13];
			swap_state[12] = _state[9];
			swap_state[13] = _state[5];
			swap_state[14] = _state[6];
			swap_state[15] = _state[10];
		break;
		case 4:
			swap_state[0] = _state[0];
			swap_state[1] = _state[4];
			swap_state[2] = _state[8];
			swap_state[3] = _state[12];
			swap_state[4] = _state[13];
			swap_state[5] = _state[14];
			swap_state[6] = _state[15];
			swap_state[7] = _state[11];
			swap_state[8] = _state[7];
			swap_state[9] = _state[3];
			swap_state[10] = _state[2];
			swap_state[11] = _state[1];
			swap_state[12] = _state[5];
			swap_state[13] = _state[9];
			swap_state[14] = _state[10];
			swap_state[15] = _state[6];
		break;
		case 5:
			swap_state[0] = _state[3];
			swap_state[1] = _state[2];
			swap_state[2] = _state[1];
			swap_state[3] = _state[0];
			swap_state[4] = _state[4];
			swap_state[5] = _state[8];
			swap_state[6] = _state[12];
			swap_state[7] = _state[13];
			swap_state[8] = _state[14];
			swap_state[9] = _state[15];
			swap_state[10] = _state[11];
			swap_state[11] = _state[7];
			swap_state[12] = _state[6];
			swap_state[13] = _state[5];
			swap_state[14] = _state[9];
			swap_state[15] = _state[10];
		break;
		case 6:
			swap_state[0] = _state[15];
			swap_state[1] = _state[11];
			swap_state[2] = _state[7];
			swap_state[3] = _state[3];
			swap_state[4] = _state[2];
			swap_state[5] = _state[1];
			swap_state[6] = _state[0];
			swap_state[7] = _state[4];
			swap_state[8] = _state[8];
			swap_state[9] = _state[12];
			swap_state[10] = _state[13];
			swap_state[11] = _state[14];
			swap_state[12] = _state[10];
			swap_state[13] = _state[6];
			swap_state[14] = _state[5];
			swap_state[15] = _state[9];
		break;		
		case 7:
			swap_state[0] = _state[12];
			swap_state[1] = _state[13];
			swap_state[2] = _state[14];
			swap_state[3] = _state[15];
			swap_state[4] = _state[11];
			swap_state[5] = _state[7];
			swap_state[6] = _state[3];
			swap_state[7] = _state[2];
			swap_state[8] = _state[1];
			swap_state[9] = _state[0];
			swap_state[10] = _state[4];
			swap_state[11] = _state[8];
			swap_state[12] = _state[9];
			swap_state[13] = _state[10];
			swap_state[14] = _state[6];
			swap_state[15] = _state[5];
		break;		
		case 8:
			swap_state[0] = _state[5];
			swap_state[1] = _state[6];
			swap_state[2] = _state[10];
			swap_state[3] = _state[9];
			swap_state[4] = _state[8];
			swap_state[5] = _state[4];
			swap_state[6] = _state[0];
			swap_state[7] = _state[1];
			swap_state[8] = _state[2];
			swap_state[9] = _state[3];
			swap_state[10] = _state[7];
			swap_state[11] = _state[11];
			swap_state[12] = _state[15];
			swap_state[13] = _state[14];
			swap_state[14] = _state[13];
			swap_state[15] = _state[12];
		break;
		case 9:
			swap_state[0] = _state[6];
			swap_state[1] = _state[10];
			swap_state[2] = _state[9];
			swap_state[3] = _state[5];
			swap_state[4] = _state[1];
			swap_state[5] = _state[2];
			swap_state[6] = _state[3];
			swap_state[7] = _state[7];
			swap_state[8] = _state[11];
			swap_state[9] = _state[15];
			swap_state[10] = _state[14];
			swap_state[11] = _state[13];
			swap_state[12] = _state[12];
			swap_state[13] = _state[8];
			swap_state[14] = _state[4];
			swap_state[15] = _state[0];
		break;
		case 10:
			swap_state[0] = _state[10];
			swap_state[1] = _state[9];
			swap_state[2] = _state[5];
			swap_state[3] = _state[6];
			swap_state[4] = _state[7];
			swap_state[5] = _state[11];
			swap_state[6] = _state[15];
			swap_state[7] = _state[14];
			swap_state[8] = _state[13];
			swap_state[9] = _state[12];
			swap_state[10] = _state[8];
			swap_state[11] = _state[4];
			swap_state[12] = _state[0];
			swap_state[13] = _state[1];
			swap_state[14] = _state[2];
			swap_state[15] = _state[3];
		break;
		case 11:
			swap_state[0] = _state[9];
			swap_state[1] = _state[5];
			swap_state[2] = _state[6];
			swap_state[3] = _state[10];
			swap_state[4] = _state[14];
			swap_state[5] = _state[13];
			swap_state[6] = _state[12];
			swap_state[7] = _state[8];
			swap_state[8] = _state[4];
			swap_state[9] = _state[0];
			swap_state[10] = _state[1];
			swap_state[11] = _state[2];
			swap_state[12] = _state[3];
			swap_state[13] = _state[7];
			swap_state[14] = _state[11];
			swap_state[15] = _state[15];
		break;
		case 12:
			swap_state[0] = _state[5];
			swap_state[1] = _state[9];
			swap_state[2] = _state[10];
			swap_state[3] = _state[6];
			swap_state[4] = _state[2];
			swap_state[5] = _state[1];
			swap_state[6] = _state[0];
			swap_state[7] = _state[4];
			swap_state[8] = _state[8];
			swap_state[9] = _state[12];
			swap_state[10] = _state[13];
			swap_state[11] = _state[14];
			swap_state[12] = _state[15];
			swap_state[13] = _state[11];
			swap_state[14] = _state[7];
			swap_state[15] = _state[3];
		break;
		case 13:
			swap_state[0] = _state[6];
			swap_state[1] = _state[5];
			swap_state[2] = _state[9];
			swap_state[3] = _state[10];
			swap_state[4] = _state[11];
			swap_state[5] = _state[7];
			swap_state[6] = _state[3];
			swap_state[7] = _state[2];
			swap_state[8] = _state[1];
			swap_state[9] = _state[0];
			swap_state[10] = _state[4];
			swap_state[11] = _state[8];
			swap_state[12] = _state[12];
			swap_state[13] = _state[13];
			swap_state[14] = _state[14];
			swap_state[15] = _state[15];
		break;
		case 14:
			swap_state[0] = _state[10];
			swap_state[1] = _state[6];
			swap_state[2] = _state[5];
			swap_state[3] = _state[9];
			swap_state[4] = _state[13];
			swap_state[5] = _state[14];
			swap_state[6] = _state[15];
			swap_state[7] = _state[11];
			swap_state[8] = _state[7];
			swap_state[9] = _state[3];
			swap_state[10] = _state[2];
			swap_state[11] = _state[1];
			swap_state[12] = _state[0];
			swap_state[13] = _state[4];
			swap_state[14] = _state[8];
			swap_state[15] = _state[12];
		break;		
		case 15:
			swap_state[0] = _state[9];
			swap_state[1] = _state[10];
			swap_state[2] = _state[6];
			swap_state[3] = _state[5];
			swap_state[4] = _state[4];
			swap_state[5] = _state[8];
			swap_state[6] = _state[12];
			swap_state[7] = _state[13];
			swap_state[8] = _state[14];
			swap_state[9] = _state[15];
			swap_state[10] = _state[11];
			swap_state[11] = _state[7];
			swap_state[12] = _state[3];
			swap_state[13] = _state[2];
			swap_state[14] = _state[1];
			swap_state[15] = _state[0];
		break;
		default:
		break;
	}
}

void spiralSWAP(uint8_t state[BLOCK_SIZE], const uint8_t type)
{
	assert(state != NULL);
	uint8_t _state[BLOCK_SIZE] = {0};
	uint8_t swap_state[BLOCK_SIZE] = {0};
	memcpy(_state, state, sizeof(_state));
	
	_Swap(_state,swap_state,type);

	memcpy(state, swap_state, sizeof(_state));
}

void inv_Swap(uint8_t _state[BLOCK_SIZE], uint8_t inv_swap_state[BLOCK_SIZE], const uint8_t type)
{
	switch (type)
	{
		case 0:
			inv_swap_state[0] = _state[0];
			inv_swap_state[1] = _state[1];
			inv_swap_state[2] = _state[2];
			inv_swap_state[3] = _state[3];
			inv_swap_state[4] = _state[11];
			inv_swap_state[5] = _state[12];
			inv_swap_state[6] = _state[13];
			inv_swap_state[7] = _state[4];
			inv_swap_state[8] = _state[10];
			inv_swap_state[9] = _state[15];
			inv_swap_state[10] = _state[14];
			inv_swap_state[11] = _state[5];
			inv_swap_state[12] = _state[9];
			inv_swap_state[13] = _state[8];
			inv_swap_state[14] = _state[7];
			inv_swap_state[15] = _state[6];
		break;
		case 1:
			inv_swap_state[0] = _state[9];
			inv_swap_state[1] = _state[10];
			inv_swap_state[2] = _state[11];
			inv_swap_state[3] = _state[0];
			inv_swap_state[4] = _state[8];
			inv_swap_state[5] = _state[15];
			inv_swap_state[6] = _state[12];
			inv_swap_state[7] = _state[1];
			inv_swap_state[8] = _state[7];
			inv_swap_state[9] = _state[14];
			inv_swap_state[10] = _state[13];
			inv_swap_state[11] = _state[2];
			inv_swap_state[12] = _state[6];
			inv_swap_state[13] = _state[5];
			inv_swap_state[14] = _state[4];
			inv_swap_state[15] = _state[3];			
		break;	
		case 2:	
			inv_swap_state[0] = _state[6];
			inv_swap_state[1] = _state[7];
			inv_swap_state[2] = _state[8];
			inv_swap_state[3] = _state[9];
			inv_swap_state[4] = _state[5];
			inv_swap_state[5] = _state[14];
			inv_swap_state[6] = _state[15];
			inv_swap_state[7] = _state[10];
			inv_swap_state[8] = _state[4];
			inv_swap_state[9] = _state[13];
			inv_swap_state[10] = _state[12];
			inv_swap_state[11] = _state[11];
			inv_swap_state[12] = _state[3];
			inv_swap_state[13] = _state[2];
			inv_swap_state[14] = _state[1];
			inv_swap_state[15] = _state[0];		
		break;		
		case 3:
			inv_swap_state[0] = _state[3];
			inv_swap_state[1] = _state[4];
			inv_swap_state[2] = _state[5];
			inv_swap_state[3] = _state[6];
			inv_swap_state[4] = _state[2];
			inv_swap_state[5] = _state[13];
			inv_swap_state[6] = _state[14];
			inv_swap_state[7] = _state[7];
			inv_swap_state[8] = _state[1];
			inv_swap_state[9] = _state[12];
			inv_swap_state[10] = _state[15];
			inv_swap_state[11] = _state[8];
			inv_swap_state[12] = _state[0];
			inv_swap_state[13] = _state[11];
			inv_swap_state[14] = _state[10];
			inv_swap_state[15] = _state[9];		
		break;
		case 4:
			inv_swap_state[0] = _state[0];
			inv_swap_state[1] = _state[11];
			inv_swap_state[2] = _state[10];
			inv_swap_state[3] = _state[9];
			inv_swap_state[4] = _state[1];
			inv_swap_state[5] = _state[12];
			inv_swap_state[6] = _state[15];
			inv_swap_state[7] = _state[8];
			inv_swap_state[8] = _state[2];
			inv_swap_state[9] = _state[13];
			inv_swap_state[10] = _state[14];
			inv_swap_state[11] = _state[7];
			inv_swap_state[12] = _state[3];
			inv_swap_state[13] = _state[4];
			inv_swap_state[14] = _state[5];
			inv_swap_state[15] = _state[6];			
		break;
		case 5:		
			inv_swap_state[0] = _state[3];
			inv_swap_state[1] = _state[2];
			inv_swap_state[2] = _state[1];
			inv_swap_state[3] = _state[0];
			inv_swap_state[4] = _state[4];
			inv_swap_state[5] = _state[13];
			inv_swap_state[6] = _state[12];
			inv_swap_state[7] = _state[11];
			inv_swap_state[8] = _state[5];
			inv_swap_state[9] = _state[14];
			inv_swap_state[10] = _state[15];
			inv_swap_state[11] = _state[10];
			inv_swap_state[12] = _state[6];
			inv_swap_state[13] = _state[7];
			inv_swap_state[14] = _state[8];
			inv_swap_state[15] = _state[9];			
		break;
		case 6:		
			inv_swap_state[0] = _state[6];
			inv_swap_state[1] = _state[5];
			inv_swap_state[2] = _state[4];
			inv_swap_state[3] = _state[3];
			inv_swap_state[4] = _state[7];
			inv_swap_state[5] = _state[14];
			inv_swap_state[6] = _state[13];
			inv_swap_state[7] = _state[2];
			inv_swap_state[8] = _state[8];
			inv_swap_state[9] = _state[15];
			inv_swap_state[10] = _state[12];
			inv_swap_state[11] = _state[1];
			inv_swap_state[12] = _state[9];
			inv_swap_state[13] = _state[10];
			inv_swap_state[14] = _state[11];
			inv_swap_state[15] = _state[0];			
		break;		
		case 7:			
			inv_swap_state[0] = _state[9];
			inv_swap_state[1] = _state[8];
			inv_swap_state[2] = _state[7];
			inv_swap_state[3] = _state[6];
			inv_swap_state[4] = _state[10];
			inv_swap_state[5] = _state[15];
			inv_swap_state[6] = _state[14];
			inv_swap_state[7] = _state[5];
			inv_swap_state[8] = _state[11];
			inv_swap_state[9] = _state[12];
			inv_swap_state[10] = _state[13];
			inv_swap_state[11] = _state[4];
			inv_swap_state[12] = _state[0];
			inv_swap_state[13] = _state[1];
			inv_swap_state[14] = _state[2];
			inv_swap_state[15] = _state[3];			
		break;		
		case 8:	
			inv_swap_state[0] = _state[6];
			inv_swap_state[1] = _state[7];
			inv_swap_state[2] = _state[8];
			inv_swap_state[3] = _state[9];
			inv_swap_state[4] = _state[5];
			inv_swap_state[5] = _state[0];
			inv_swap_state[6] = _state[1];
			inv_swap_state[7] = _state[10];
			inv_swap_state[8] = _state[4];
			inv_swap_state[9] = _state[3];
			inv_swap_state[10] = _state[2];
			inv_swap_state[11] = _state[11];
			inv_swap_state[12] = _state[15];
			inv_swap_state[13] = _state[14];
			inv_swap_state[14] = _state[13];
			inv_swap_state[15] = _state[12];			
		break;
		case 9:			
			inv_swap_state[0] = _state[15];
			inv_swap_state[1] = _state[4];
			inv_swap_state[2] = _state[5];
			inv_swap_state[3] = _state[6];
			inv_swap_state[4] = _state[14];
			inv_swap_state[5] = _state[3];
			inv_swap_state[6] = _state[0];
			inv_swap_state[7] = _state[7];
			inv_swap_state[8] = _state[13];
			inv_swap_state[9] = _state[2];
			inv_swap_state[10] = _state[1];
			inv_swap_state[11] = _state[8];
			inv_swap_state[12] = _state[12];
			inv_swap_state[13] = _state[11];
			inv_swap_state[14] = _state[10];
			inv_swap_state[15] = _state[9];			
		break;
		case 10:
			inv_swap_state[0] = _state[12];
			inv_swap_state[1] = _state[13];
			inv_swap_state[2] = _state[14];
			inv_swap_state[3] = _state[15];
			inv_swap_state[4] = _state[11];
			inv_swap_state[5] = _state[2];
			inv_swap_state[6] = _state[3];
			inv_swap_state[7] = _state[4];
			inv_swap_state[8] = _state[10];
			inv_swap_state[9] = _state[1];
			inv_swap_state[10] = _state[0];
			inv_swap_state[11] = _state[5];
			inv_swap_state[12] = _state[9];
			inv_swap_state[13] = _state[8];
			inv_swap_state[14] = _state[7];
			inv_swap_state[15] = _state[6];			
		break;
		case 11:	
			inv_swap_state[0] = _state[9];
			inv_swap_state[1] = _state[10];
			inv_swap_state[2] = _state[11];
			inv_swap_state[3] = _state[12];
			inv_swap_state[4] = _state[8];
			inv_swap_state[5] = _state[1];
			inv_swap_state[6] = _state[2];
			inv_swap_state[7] = _state[13];
			inv_swap_state[8] = _state[7];
			inv_swap_state[9] = _state[0];
			inv_swap_state[10] = _state[3];
			inv_swap_state[11] = _state[14];
			inv_swap_state[12] = _state[6];
			inv_swap_state[13] = _state[5];
			inv_swap_state[14] = _state[4];
			inv_swap_state[15] = _state[15];			
		break;
		case 12:			
			inv_swap_state[0] = _state[6];
			inv_swap_state[1] = _state[5];
			inv_swap_state[2] = _state[4];
			inv_swap_state[3] = _state[15];
			inv_swap_state[4] = _state[7];
			inv_swap_state[5] = _state[0];
			inv_swap_state[6] = _state[3];
			inv_swap_state[7] = _state[14];
			inv_swap_state[8] = _state[8];
			inv_swap_state[9] = _state[1];
			inv_swap_state[10] = _state[2];
			inv_swap_state[11] = _state[13];
			inv_swap_state[12] = _state[9];
			inv_swap_state[13] = _state[10];
			inv_swap_state[14] = _state[11];
			inv_swap_state[15] = _state[12];			
		break;
		case 13:			
			inv_swap_state[0] = _state[9];
			inv_swap_state[1] = _state[8];
			inv_swap_state[2] = _state[7];
			inv_swap_state[3] = _state[6];
			inv_swap_state[4] = _state[10];
			inv_swap_state[5] = _state[1];
			inv_swap_state[6] = _state[0];
			inv_swap_state[7] = _state[5];
			inv_swap_state[8] = _state[11];
			inv_swap_state[9] = _state[2];
			inv_swap_state[10] = _state[3];
			inv_swap_state[11] = _state[4];
			inv_swap_state[12] = _state[12];
			inv_swap_state[13] = _state[13];
			inv_swap_state[14] = _state[14];
			inv_swap_state[15] = _state[15];			
		break;
		case 14:	
			inv_swap_state[0] = _state[12];
			inv_swap_state[1] = _state[11];
			inv_swap_state[2] = _state[10];
			inv_swap_state[3] = _state[9];
			inv_swap_state[4] = _state[13];
			inv_swap_state[5] = _state[2];
			inv_swap_state[6] = _state[1];
			inv_swap_state[7] = _state[8];
			inv_swap_state[8] = _state[14];
			inv_swap_state[9] = _state[3];
			inv_swap_state[10] = _state[0];
			inv_swap_state[11] = _state[7];
			inv_swap_state[12] = _state[15];
			inv_swap_state[13] = _state[4];
			inv_swap_state[14] = _state[5];
			inv_swap_state[15] = _state[6];			
		break;		
		case 15:
			inv_swap_state[0] = _state[15];
			inv_swap_state[1] = _state[14];
			inv_swap_state[2] = _state[13];
			inv_swap_state[3] = _state[12];
			inv_swap_state[4] = _state[4];
			inv_swap_state[5] = _state[3];
			inv_swap_state[6] = _state[2];
			inv_swap_state[7] = _state[11];
			inv_swap_state[8] = _state[5];
			inv_swap_state[9] = _state[0];
			inv_swap_state[10] = _state[1];
			inv_swap_state[11] = _state[10];
			inv_swap_state[12] = _state[6];
			inv_swap_state[13] = _state[7];
			inv_swap_state[14] = _state[8];
			inv_swap_state[15] = _state[9];			
		break;
		default:
		break;
	}	
}
void inv_spiralSWAP(uint8_t state[BLOCK_SIZE], const uint8_t type)
{
	assert(state != NULL);
	uint8_t _state[BLOCK_SIZE] = {0};
	uint8_t inv_swap_state[BLOCK_SIZE] = {0};
	memcpy(_state, state, sizeof(_state));
	//printf("inv_spiralSWAP \n");
	inv_Swap(_state,inv_swap_state,type);

	memcpy(state, inv_swap_state, sizeof(_state));
}

static void Maes_round(uint8_t state[BLOCK_SIZE], const uint8_t rk[BLOCK_SIZE])
{
	Csub_bytes(state);
	//sub_bytes(state);
	//shift_rows(state);
	spiralSWAP(state,swap_type);	
	mix_columns(state);
	add_round_key(state, rk);
}
static void Maes_inv_round(uint8_t state[BLOCK_SIZE], const uint8_t inv_rk[BLOCK_SIZE])
{
	//inv_shift_rows(state);
	inv_spiralSWAP(state,swap_type);	
	Cinv_sub_bytes(state);
	//inv_sub_bytes(state);
	add_round_key(state, inv_rk);
	inv_mix_columns(state);
}

static void Maes_final_round(uint8_t state[BLOCK_SIZE], const uint8_t rk[BLOCK_SIZE])
{
	Csub_bytes(state);
	//sub_bytes(state);
	//shift_rows(state);
	spiralSWAP(state,swap_type);
	add_round_key(state, rk);
}
static void Minv_final_round(uint8_t state[BLOCK_SIZE], const uint8_t inv_rk[BLOCK_SIZE])
{
	//inv_shift_rows(state);
	inv_spiralSWAP(state,swap_type);
	Cinv_sub_bytes(state);
	//inv_sub_bytes(state);
	add_round_key(state, inv_rk);
}



int Maes_encrypt_block(aes_context *ctx
	, uint8_t cipher_text[BLOCK_SIZE], const uint8_t text[BLOCK_SIZE])
{
	if (ctx == NULL || cipher_text == NULL || text == NULL)
		return PARM_ERROR;
	if (ctx->rk != ctx->buf)
		return NOT_INIT_KEY;
	uint32_t Nr = ctx->nr;
	uint32_t *RK = ctx->rk;
	uint8_t *state = cipher_text;
	memcpy(state, text, BLOCK_SIZE);
	//printf("Maes_encrypt_block Nr = %d\n",Nr);
	add_round_key(state, (const uint8_t *)RK);
	uint32_t i = 1;
	for (i = 1; i < Nr-6; ++i)
		Maes_round(state, (const uint8_t *)(RK + (i<<2)));
	//Maes_round(state, (const uint8_t *)(RK + (i<<2)));
	Maes_final_round(state, (const uint8_t *)(RK + (Nr<<2)));
	
	return SUCCESS;
}

int Maes_decrypt_block(aes_context *ctx
	, uint8_t text[BLOCK_SIZE], const uint8_t cipher_text[BLOCK_SIZE])
{
	if (ctx == NULL || text == NULL || cipher_text == NULL)
		return PARM_ERROR;
	if (ctx->rk != ctx->buf)
		return NOT_INIT_KEY;
	uint32_t Nr = ctx->nr;
	uint32_t *INV_RK = ctx->rk;
	uint8_t *state = text;
	memcpy(state, cipher_text, BLOCK_SIZE);
	//printf("Maes_decrypt_block Nr = %d\n",Nr);
	add_round_key(state, (const uint8_t *)(INV_RK + (Nr<<2)));
	uint32_t i = 1;
	for (i = Nr-1-6; i > 0; --i)
		Maes_inv_round(state, (const uint8_t *)(INV_RK + (i<<2)));
	//Maes_inv_round(state, (const uint8_t *)(INV_RK + (i<<2)));
	Minv_final_round(state, (const uint8_t *)INV_RK);
	
	return SUCCESS;
}
