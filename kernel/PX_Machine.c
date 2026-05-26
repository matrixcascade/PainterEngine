#include "PX_Machine.h"

#define PX_MACHINE_DEFAULT_RUNTIME_MEMORY_SIZE (1024 * 1024)
#define PX_MACHINE_DEFAULT_THREAD_STACK_SIZE   (128 * 1024)



px_void PX_Machine_Crash(PX_Machine* pmac, const px_char* message)
{
	PX_Machine_Message(pmac, message);
	pmac->state = PX_MACHINE_STATE_ERROR;
}



px_void PX_Machine_Execute_Opcode_loadxx(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip,px_bool is_signed,px_int x)
{
	px_dword target_reg;
	px_bool is_global;
	px_dword offset;
	px_dword address;
	if (pthread->ip+6>=pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_loadu8: thread ip out of range");
		return;
	}
	target_reg = ip[1] & 0x0f;
	is_global = (ip[1] & 0x10) ? PX_FALSE:PX_TRUE;
	offset = *(px_dword*)(ip + 2);
	if (!is_global)
	{
		address = pthread->bp - offset;
	}
	else
	{
		address = offset;
	}
	if (address + x / 8 > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_loadu8: load address out of range");
		return;
	}
	if (target_reg>1)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_loadu8: target register out of range");
		return;
	}
	if (is_signed)
	{
		switch (x)
		{
			case 8:
				pthread->r[target_reg] = (px_dword)(px_char)pmac->runtime_memory[address];
			break;
			case 16:
				pthread->r[target_reg] = (px_dword)(px_short)(*(px_word*)&pmac->runtime_memory[address]);
			break;
			case 32:
				pthread->r[target_reg] = (*(px_dword*)&pmac->runtime_memory[address]);
			break;
			default:
				PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_loadxx: x error");
				return;
		}
	}
	else
	{
		switch (x)
		{
		case 8:
			pthread->r[target_reg] = (px_dword)(px_byte)pmac->runtime_memory[address];
			break;
		case 16:
			pthread->r[target_reg] = (px_dword)(px_word)(*(px_word*)&pmac->runtime_memory[address]);
			break;
		case 32:
			pthread->r[target_reg] = (*(px_dword*)&pmac->runtime_memory[address]);
			break;
		default:
			PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_loadxx: x error");
			return;
		}
	}
	pthread->ip += 6;
}

px_void PX_Machine_Execute_Opcode_storex(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip,  px_int x)
{
	px_dword source_reg;
	px_bool is_global;
	px_dword offset;
	px_dword address;
	if (pthread->ip + 6 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_storex: thread ip out of range");
		return;
	}
	source_reg = ip[1] & 0x0f;
	is_global = (ip[1] & 0x10) ?  PX_FALSE:PX_TRUE;
	offset = *(px_dword*)(ip + 2);
	if (!is_global)
	{
		address = pthread->bp - offset;
	}
	else
	{
		address = offset;
	}
	if (address + x / 8 > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_storex: store address out of range");
		return;
	}
	if (source_reg > 1)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_storex: source register out of range");
		return;
	}
	switch (x)
	{
	case 8:
		pmac->runtime_memory[address] = (px_byte)(pthread->r[source_reg] & 0xFF);
		break;
	case 16:
		*(px_word*)&pmac->runtime_memory[address] = (px_word)(pthread->r[source_reg] & 0xFFFF);
		break;
	case 32:
		*(px_dword*)&pmac->runtime_memory[address] = pthread->r[source_reg];
		break;
	default:
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_storex: x error");
		return;
	}
	pthread->ip += 6;

}

px_void PX_Machine_Execute_Opcode_pushad(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int i;
	if (pthread->ip + 1 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_pushad: thread ip out of range");
		return;
	}
	pthread->sp -= 4*8;
	if (pthread->sp > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_pushad: stack overflow");
		return;
	}
	for (i = 0; i < 8; i++)
	{
		*(px_dword*)&pmac->runtime_memory[pthread->sp + i * 4] = pthread->r[i];
	}
	pthread->ip += 1;
}

px_void PX_Machine_Execute_Opcode_popad(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int i;
	if (pthread->ip + 1 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_popad: thread ip out of range");
		return;
	}
	if (pthread->sp + 4 * 8 > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_popad: stack underflow");
		return;
	}
	for (i = 0; i < 8; i++)
	{
		pthread->r[i] = *(px_dword*)&pmac->runtime_memory[pthread->sp + i * 4];
	}
	pthread->sp += 4 * 8;
	pthread->ip += 1;
}

px_void PX_Machine_Execute_Opcode_movr(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_movr: thread ip out of range");
		return;
	}
	do
	{
		px_bool dst_is_float = (px_bool)((ip[1] & 0x01) ? PX_TRUE : PX_FALSE);
		px_int dst_index = (px_int)((ip[1] & 0xf0) >> 4);
		px_bool src_is_float = (px_bool)((ip[2] & 0x01) ? PX_TRUE : PX_FALSE);
		px_int src_index = (px_int)((ip[2] & 0xf0) >> 4);
		if (dst_is_float)
		{
			if (dst_index > 3)
			{
				PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_movr: dst float register out of range");
				return;
			}
			if (src_is_float)
			{
				if (src_index > 3)
				{
					PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_movr: src float register out of range");
					return;
				}
				pthread->f[dst_index] = pthread->f[src_index];
			}
			else
			{
				if (src_index > 7)
				{
					PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_movr: src register out of range");
					return;
				}
				pthread->f[dst_index] = *((px_float*)&pthread->r[src_index]);
			}
		}
		else
		{
			if (dst_index > 7)
			{
				PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_movr: dst register out of range");
				return;
			}
			if (src_is_float)
			{
				if (src_index > 3)
				{
					PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_movr: src float register out of range");
					return;
				}
				pthread->r[dst_index] = *((px_dword*)&pthread->f[src_index]);
			}
			else
			{
				if (src_index > 7)
				{
					PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_movr: src register out of range");
					return;
				}
				pthread->r[dst_index] = pthread->r[src_index];
			}
		}
	} while (0);
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_movc_movf(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	if (pthread->ip + 6 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_movc/movf: thread ip out of range");
		return;
	}
	do
	{
		px_int dst_index = (px_int)((ip[1] & 0x07));
		px_dword constant_value = *(px_dword*)(ip + 2);
		if (dst_index>7)
		{
			PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_movc/movf: dst register out of range");
			return;
		}
		pthread->r[dst_index] = constant_value;
	} while (0);
	pthread->ip += 6;
}

px_void PX_Machine_Execute_Opcode_movn(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	if (pthread->ip + 1 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_movc/movf: thread ip out of range");
		return;
	}
	if ((pthread->r0 + pthread->r2 > pmac->runtime_memory_size)||(pthread->r1+ pthread->r2> pmac->runtime_memory_size))
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_movc/movn: memory copy out of range");
		return;
	}
	PX_memcpy(pmac->runtime_memory + pthread->r1, pmac->runtime_memory + pthread->r0, pthread->r2);
	pthread->ip += 1;
}

px_void PX_Machine_Execute_Opcode_f2i(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	if (pthread->ip + 2 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_f2i: thread ip out of range");
		return;
	}
	do
	{
		px_int dst_index = (px_int)((ip[1] & 0x0f));
		px_int src_index = (px_int)((ip[1] & 0xf0) >>4);
		if (dst_index >1)
		{
			PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_f2i: dst register out of range");
			return;
		}
		if (src_index >1)
		{
			PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_f2i: src float register out of range");
			return;
		}
		pthread->r[dst_index] = (px_dword)(px_int)(pthread->f[src_index]);
	} while (0);
	pthread->ip += 2;
}

px_void PX_Machine_Execute_Opcode_f2u(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	if (pthread->ip + 2 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_f2u: thread ip out of range");
		return;
	}
	do
	{
		px_int dst_index = (px_int)((ip[1] & 0x0f));
		px_int src_index = (px_int)((ip[1] & 0xf0) >>4);
		if (dst_index >1)
		{
			PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_f2u: dst register out of range");
			return;
		}
		if (src_index >1)
		{
			PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_f2u: src float register out of range");
			return;
		}
		pthread->r[dst_index] = (px_dword)(pthread->f[src_index]);
	} while (0);
	pthread->ip += 2;
}

px_void PX_Machine_Execute_Opcode_i2f(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	if (pthread->ip + 2 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_i2f: thread ip out of range");
		return;
	}
	do
	{
		px_int dst_index = (px_int)((ip[1] & 0x0f));
		px_int src_index = (px_int)((ip[1] & 0xf0) >> 4);
		if (dst_index > 1)
		{
			PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_i2f: dst float register out of range");
			return;
		}
		if (src_index > 1)
		{
			PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_i2f: src register out of range");
			return;
		}
		pthread->f[dst_index] = (px_float)(*(px_int *)&pthread->r[src_index]);
	} while (0);
	pthread->ip += 2;

}

px_void PX_Machine_Execute_Opcode_u2f(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	if (pthread->ip + 2 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_i2f: thread ip out of range");
		return;
	}
	do
	{
		px_int dst_index = (px_int)((ip[1] & 0x0f));
		px_int src_index = (px_int)((ip[1] & 0xf0) >> 4);
		if (dst_index > 1)
		{
			PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_i2f: dst float register out of range");
			return;
		}
		if (src_index > 1)
		{
			PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_i2f: src register out of range");
			return;
		}
		pthread->f[dst_index] = (px_float)(pthread->r[src_index]);
	} while (0);
	pthread->ip += 2;

}

px_void PX_Machine_Execute_Opcode_neg(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	if (pthread->ip + 2 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_neg: thread ip out of range");
		return;
	}
	do
	{
		px_int reg_index = ip[1];
		if (reg_index > 1)
		{
			PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_neg: register out of range");
			return;
		}
		pthread->r[reg_index] = (~pthread->r[reg_index]) + 1;
	} while (0);
	pthread->ip += 2;
}

px_void PX_Machine_Execute_Opcode_add(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int dst_index, src1_index, src2_index;
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_neg: thread ip out of range");
		return;
	}
	dst_index = (px_int)((ip[1] & 0x0f));
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[2] & 0x0f));
	if (dst_index > 7 || src1_index > 7 || src2_index > 7)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_add: register out of range");
		return;
	}
	pthread->r[dst_index] = pthread->r[src1_index] + pthread->r[src2_index];
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_sub(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int dst_index, src1_index, src2_index;
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_neg: thread ip out of range");
		return;
	}
	dst_index = (px_int)((ip[1] & 0x0f));
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[2] & 0x0f));
	if (dst_index > 7 || src1_index > 7 || src2_index > 7)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_sub: register out of range");
		return;
	}
	pthread->r[dst_index] = pthread->r[src1_index] - pthread->r[src2_index];
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_mul(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int dst_index, src1_index, src2_index;
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_neg: thread ip out of range");
		return;
	}
	dst_index = (px_int)((ip[1] & 0x0f));
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[2] & 0x0f));
	if (dst_index > 7 || src1_index > 7 || src2_index > 7)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_mul: register out of range");
		return;
	}
	pthread->r[dst_index] = pthread->r[src1_index] * pthread->r[src2_index];
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_div(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int dst_index, src1_index, src2_index;
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_neg: thread ip out of range");
		return;
	}
	dst_index = (px_int)((ip[1] & 0x0f));
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[2] & 0x0f));
	if (dst_index > 7 || src1_index > 7 || src2_index > 7)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_div: register out of range");
		return;
	}
	if (pthread->r[src2_index] == 0)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_div: divide by zero");
		return;
	}
	pthread->r[dst_index] = pthread->r[src1_index] / pthread->r[src2_index];
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_idiv(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int dst_index, src1_index, src2_index;
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_neg: thread ip out of range");
		return;
	}
	dst_index = (px_int)((ip[1] & 0x0f));
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[2] & 0x0f));
	if (dst_index > 7 || src1_index > 7 || src2_index > 7)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_idiv: register out of range");
		return;
	}
	if ((px_int)pthread->r[src2_index] == 0)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_idiv: divide by zero");
		return;
	}
	pthread->r[dst_index] = (px_dword)((px_int)pthread->r[src1_index] / (px_int)pthread->r[src2_index]);
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_mod(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int dst_index, src1_index, src2_index;
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_neg: thread ip out of range");
		return;
	}
	dst_index = (px_int)((ip[1] & 0x0f));
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[2] & 0x0f));
	if (dst_index > 7 || src1_index > 7 || src2_index > 7)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_mod: register out of range");
		return;
	}
	if (pthread->r[src2_index] == 0)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_mod: divide by zero");
		return;
	}
	pthread->r[dst_index] = pthread->r[src1_index] % pthread->r[src2_index];
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_fneg(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	if (pthread->ip + 2 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fneg: thread ip out of range");
		return;
	}
	do
	{
		px_int reg_index = ip[1];
		if (reg_index > 3)
		{
			PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fneg: float register out of range");
			return;
		}
		pthread->f[reg_index] = -pthread->f[reg_index];
	} while (0);
	pthread->ip += 2;
}

px_void PX_Machine_Execute_Opcode_fadd(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int dst_index, src1_index, src2_index;
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fadd: thread ip out of range");
		return;
	}
	dst_index = (px_int)((ip[1] & 0x0f));
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[2] & 0x0f));
	if (dst_index > 3 || src1_index > 3 || src2_index > 3)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fadd: float register out of range");
		return;
	}
	pthread->f[dst_index] = pthread->f[src1_index] + pthread->f[src2_index];
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_fsub(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int dst_index, src1_index, src2_index;
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fsub: thread ip out of range");
		return;
	}
	dst_index = (px_int)((ip[1] & 0x0f));
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[2] & 0x0f));
	if (dst_index > 3 || src1_index > 3 || src2_index > 3)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fsub: float register out of range");
		return;
	}
	pthread->f[dst_index] = pthread->f[src1_index] - pthread->f[src2_index];
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_fmul(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int dst_index, src1_index, src2_index;
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fmul: thread ip out of range");
		return;
	}
	dst_index = (px_int)((ip[1] & 0x0f));
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[2] & 0x0f));
	if (dst_index > 3 || src1_index > 3 || src2_index > 3)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fmul: float register out of range");
		return;
	}
	pthread->f[dst_index] = pthread->f[src1_index] * pthread->f[src2_index];
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_fdiv(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int dst_index, src1_index, src2_index;
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fdiv: thread ip out of range");
		return;
	}
	dst_index = (px_int)((ip[1] & 0x0f));
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[2] & 0x0f));
	if (dst_index > 3 || src1_index > 3 || src2_index > 3)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fdiv: float register out of range");
		return;
	}
	if (pthread->f[src2_index] == 0)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fdiv: divide by zero");
		return;
	}
	pthread->f[dst_index] = pthread->f[src1_index] / pthread->f[src2_index];
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_fcmp(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int src1_index, src2_index;
	if (pthread->ip + 2 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fcmp: thread ip out of range");
		return;
	}
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[1] & 0x0f));
	if (src1_index > 3 || src2_index > 3)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fcmp: float register out of range");
		return;
	}
	if (pthread->f[src1_index] == pthread->f[src2_index])
	{
		pthread->C0 = 0;
		pthread->C2 = 0;
		pthread->C3 = 1;

	}
	else if (pthread->f[src1_index] > pthread->f[src2_index])
	{
		pthread->C0 = 0;
		pthread->C2 = 0;
		pthread->C3 = 0;
	}
	else
	{
		pthread->C0 = 1;
		pthread->C2 = 0;
		pthread->C3 = 0;
	}
	pthread->ip += 2;

}

px_void PX_Machine_Execute_Opcode_fcomi(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int src1_index, src2_index;
	if (pthread->ip + 2 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fcomi: thread ip out of range");
		return;
	}
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[1] & 0x0f));
	if (src1_index > 3 || src2_index > 3)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fcomi: float register out of range");
		return;
	}
	if (pthread->f[src1_index] == pthread->f[src2_index])
	{
		pthread->Z = PX_TRUE;
		pthread->C = PX_FALSE;
	}
	else if (pthread->f[src1_index] > pthread->f[src2_index])
	{
		pthread->Z = PX_FALSE;
		pthread->C = PX_FALSE;
	}
	else
	{
		pthread->Z = PX_FALSE;
		pthread->C = PX_TRUE;
	}
	pthread->ip += 2;
}

px_void PX_Machine_Execute_Opcode_ff2f(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	if (pthread->ip + 1 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_fstsw: thread ip out of range");
		return;
	}
	pthread->Z = pthread->C3;
	pthread->C = pthread->C0;
	pthread->ip += 1;
}

px_void PX_Machine_Execute_Opcode_and(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int dst_index, src1_index, src2_index;
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_and: thread ip out of range");
		return;
	}
	dst_index = (px_int)((ip[1] & 0x0f));
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[2] & 0x0f));
	if (dst_index > 7 || src1_index > 7 || src2_index > 7)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_and: register out of range");
		return;
	}
	pthread->r[dst_index] = pthread->r[src1_index] & pthread->r[src2_index];
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_or(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int dst_index, src1_index, src2_index;
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_or: thread ip out of range");
		return;
	}
	dst_index = (px_int)((ip[1] & 0x0f));
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[2] & 0x0f));
	if (dst_index > 7 || src1_index > 7 || src2_index > 7)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_or: register out of range");
		return;
	}
	pthread->r[dst_index] = pthread->r[src1_index] | pthread->r[src2_index];
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_xor(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int dst_index, src1_index, src2_index;
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_xor: thread ip out of range");
		return;
	}
	dst_index = (px_int)((ip[1] & 0x0f));
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[2] & 0x0f));
	if (dst_index > 7 || src1_index > 7 || src2_index > 7)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_xor: register out of range");
		return;
	}
	pthread->r[dst_index] = pthread->r[src1_index] ^ pthread->r[src2_index];
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_not(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int reg_index;
	if (pthread->ip + 2 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_not: thread ip out of range");
		return;
	}
	reg_index = (px_int)(ip[1]);
	if (reg_index > 7)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_not: register out of range");
		return;
	}
	pthread->r[reg_index] = ~pthread->r[reg_index];
	pthread->ip += 2;
}

px_void PX_Machine_Execute_Opcode_shl(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int dst_index, src1_index, src2_index;
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_xor: thread ip out of range");
		return;
	}
	dst_index = (px_int)((ip[1] & 0x0f));
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[2] & 0x0f));
	if (dst_index > 7 || src1_index > 7 || src2_index > 7)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_xor: register out of range");
		return;
	}
	pthread->r[dst_index] = pthread->r[src1_index] << pthread->r[src2_index];
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_shr(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int dst_index, src1_index, src2_index;
	if (pthread->ip + 3 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_xor: thread ip out of range");
		return;
	}
	dst_index = (px_int)((ip[1] & 0x0f));
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[2] & 0x0f));
	if (dst_index > 7 || src1_index > 7 || src2_index > 7)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_xor: register out of range");
		return;
	}
	pthread->r[dst_index] = pthread->r[src1_index] >> pthread->r[src2_index];
	pthread->ip += 3;
}

px_void PX_Machine_Execute_Opcode_cmp(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_int src1_index, src2_index;
	if (pthread->ip + 2 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_cmp: thread ip out of range");
		return;
	}
	src1_index = (px_int)((ip[1] & 0xf0) >> 4);
	src2_index = (px_int)((ip[1] & 0x0f));
	if (src1_index > 7 || src2_index > 7)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_cmp: register out of range");
		return;
	}
	pthread->Z = (px_bool)(pthread->r[src1_index] == pthread->r[src2_index] ? PX_TRUE : PX_FALSE);
	pthread->C = (px_bool)(pthread->r[src1_index] < pthread->r[src2_index] ? PX_TRUE : PX_FALSE);
	pthread->N = ((pthread->r[src1_index] - pthread->r[src2_index]) & 0x80000000) ? PX_TRUE : PX_FALSE;
	pthread->V = (pthread->r[src1_index] & 0x80000000) != (pthread->r[src2_index] & 0x80000000) \
		&& (pthread->r[src1_index] & 0x80000000) != ((pthread->r[src1_index] - pthread->r[src2_index]) & 0x80000000) ?\
		PX_TRUE : PX_FALSE;
	pthread->ip += 2;
}

px_void PX_Machine_Execute_Opcode_jmp(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_dword jmp_address;
	if (pthread->ip + 5 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_jmp: thread ip out of range");
		return;
	}
	jmp_address = *(px_dword*)(ip + 1);
	if (jmp_address>= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_jmp: jump address out of range");
		return;
	}
	pthread->ip = jmp_address;
}

px_void PX_Machine_Execute_Opcode_je(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_dword jmp_address;
	if (pthread->ip + 5 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_je: thread ip out of range");
		return;
	}
	jmp_address = *(px_dword*)(ip + 1);
	if (jmp_address >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_je: jump address out of range");
		return;
	}
	if (pthread->Z)
	{
		pthread->ip = jmp_address;
	}
	else
	{
		pthread->ip += 5;
	}
}

px_void PX_Machine_Execute_Opcode_jne(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_dword jmp_address;
	if (pthread->ip + 5 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_jne: thread ip out of range");
		return;
	}
	jmp_address = *(px_dword*)(ip + 1);
	if (jmp_address >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_jne: jump address out of range");
		return;
	}
	if (!pthread->Z)
	{
		pthread->ip = jmp_address;
	}
	else
	{
		pthread->ip += 5;
	}
}

px_void PX_Machine_Execute_Opcode_jg(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_dword jmp_address;
	if (pthread->ip + 5 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_jg: thread ip out of range");
		return;
	}
	jmp_address = *(px_dword*)(ip + 1);
	if (jmp_address >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_jg: jump address out of range");
		return;
	}
	if (!pthread->Z && (pthread->N == pthread->V))
	{
		pthread->ip = jmp_address;
	}
	else
	{
		pthread->ip += 5;
	}
}

px_void PX_Machine_Execute_Opcode_jge(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_dword jmp_address;
	if (pthread->ip + 5 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_jge: thread ip out of range");
		return;
	}
	jmp_address = *(px_dword*)(ip + 1);
	if (jmp_address >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_jge: jump address out of range");
		return;
	}
	if (pthread->N == pthread->V)
	{
		pthread->ip = jmp_address;
	}
	else
	{
		pthread->ip += 5;
	}
}

px_void PX_Machine_Execute_Opcode_jl(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_dword jmp_address;
	if (pthread->ip + 5 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_jl: thread ip out of range");
		return;
	}
	jmp_address = *(px_dword*)(ip + 1);
	if (jmp_address >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_jl: jump address out of range");
		return;
	}
	if (pthread->N != pthread->V)
	{
		pthread->ip = jmp_address;
	}
	else
	{
		pthread->ip += 5;
	}

}

px_void PX_Machine_Execute_Opcode_jle(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_dword jmp_address;
	if (pthread->ip + 5 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_jle: thread ip out of range");
		return;
	}
	jmp_address = *(px_dword*)(ip + 1);
	if (jmp_address >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_jle: jump address out of range");
		return;
	}
	if (pthread->Z || (pthread->N != pthread->V))
	{
		pthread->ip = jmp_address;
	}
	else
	{
		pthread->ip += 5;
	}
}

px_void PX_Machine_Execute_Opcode_call(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	// call: 5 bytes -- opcode + 32-bit absolute address
	px_dword offset;
	if (pthread->ip + 5 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_call: thread ip out of range");
		return;
	}
	offset = *(px_dword*)(ip + 1);
	if (offset >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_call: call address out of range");
		return;
	}
	//push return address
	pthread->sp -= 4;
	if (pthread->sp > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_call: stack overflow");
		return;
	}
	*(px_dword*)&pmac->runtime_memory[pthread->sp] = pthread->ip + 5;
	//push bp
	pthread->sp -= 4;
	if (pthread->sp > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_call: stack overflow");
		return;
	}
	*(px_dword*)&pmac->runtime_memory[pthread->sp] = pthread->bp;
	pthread->ip = offset;
}

px_void PX_Machine_Execute_Opcode_ret(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_dword return_address;
	if (pthread->sp + 8 > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ret: stack underflow");
		return;
	}
	//pop bp
	pthread->bp = *(px_dword*)&pmac->runtime_memory[pthread->sp];
	pthread->sp += 4;
	//pop return address
	return_address = *(px_dword*)&pmac->runtime_memory[pthread->sp];
	pthread->sp += 4;
	if (return_address >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ret: return address out of range");
		return;
	}
	pthread->ip = return_address;

}

px_void PX_Machine_Execute_Opcode_callr(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	// callr: 2 bytes -- opcode + register index
	px_dword call_address;
	px_int reg_index;
	if (pthread->ip + 2 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_callr: thread ip out of range");
		return;
	}
	reg_index = (px_int)(ip[1] & 0xff);
	if (reg_index > 7)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_callr: register out of range");
		return;
	}
	call_address = pthread->r[reg_index];
	if (call_address >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_callr: call address out of range");
		return;
	}
	//push return address
	pthread->sp -= 4;
	if (pthread->sp > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_callr: stack overflow");
		return;
	}
	*(px_dword*)&pmac->runtime_memory[pthread->sp] = pthread->ip + 2;
	//push bp
	pthread->sp -= 4;
	if (pthread->sp > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_callr: stack overflow");
		return;
	}
	*(px_dword*)&pmac->runtime_memory[pthread->sp] = pthread->bp;
	pthread->ip = call_address;
}

px_void PX_Machine_Execute_Opcode_jmpr(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	// jmpr: 2 bytes -- opcode + register index
	px_dword jmp_address;
	px_int reg_index;
	if (pthread->ip + 2 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_jmpr: thread ip out of range");
		return;
	}
	reg_index = (px_int)(ip[1] & 0xff);
	if (reg_index > 7)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_jmpr: register out of range");
		return;
	}
	jmp_address = pthread->r[reg_index];
	if (jmp_address >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_jmpr: jump address out of range");
		return;
	}
	pthread->ip = jmp_address;
}

px_void PX_Machine_Execute_Opcode_addsp(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	// addsp: 5 bytes -- opcode + 32-bit n
	px_dword n;
	if (pthread->ip + 5 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_addsp: thread ip out of range");
		return;
	}
	n = *(px_dword*)(ip + 1);
	pthread->sp += n;
	if (pthread->sp > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_addsp: stack pointer out of range");
		return;
	}
	pthread->ip += 5;
}

px_void PX_Machine_Execute_Opcode_subsp(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	// subsp: 5 bytes -- opcode + 32-bit n
	px_dword n;
	if (pthread->ip + 5 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_subsp: thread ip out of range");
		return;
	}
	n = *(px_dword*)(ip + 1);
	pthread->sp -= n;
	if (pthread->sp > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_subsp: stack pointer out of range");
		return;
	}
	pthread->ip += 5;
}
// ee instruction helper: get integer operand value by type
// type: 0=const_int  1=const_float  2=rx  3=fx  4=addr(global)  5=bp-addr(local)
// For const_float (type 1): value is bit-pattern of float, treated as px_dword
static px_bool PX_Machine_ee_get_int(PX_Machine* pmac, PX_Machine_Thread* pthread, px_byte type, px_dword value, px_dword* out)
{
	switch (type)
	{
	case 0: // const_int
		*out = value;
		return PX_TRUE;
	case 1: // const_float - raw bits
		*out = value;
		return PX_TRUE;
	case 2: // rx
		if (value > 7)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_get_int: rx index out of range");
			return PX_FALSE;
		}
		*out = pthread->r[value];
		return PX_TRUE;
	case 3: // fx - raw bits
		if (value > 3)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_get_int: fx index out of range");
			return PX_FALSE;
		}
		*out = *(px_dword*)&pthread->f[value];
		return PX_TRUE;
	case 4: // addr (global)
		if (value >= pmac->runtime_memory_size)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_get_int: addr out of range");
			return PX_FALSE;
		}
		*out = *(px_dword*)&pmac->runtime_memory[value];
		return PX_TRUE;
	case 5: // bp-addr (local)
	{
		px_dword real_addr = pthread->bp - value;
		if (real_addr >= pmac->runtime_memory_size)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_get_int: bp-addr out of range");
			return PX_FALSE;
		}
		*out = *(px_dword*)&pmac->runtime_memory[real_addr];
		return PX_TRUE;
	}
	default:
		PX_Machine_Crash(pmac, "PX_Machine_ee_get_int: unknown type");
		return PX_FALSE;
	}
}

// ee instruction helper: get float operand value by type
// type: 0=const_int  1=const_float  2=rx  3=fx  4=addr(global)  5=bp-addr(local)
static px_bool PX_Machine_ee_get_float(PX_Machine* pmac, PX_Machine_Thread* pthread, px_byte type, px_dword value, px_float* out)
{
	switch (type)
	{
	case 0: // const_int - treat integer bits as float bits
		*out = *(px_float*)&value;
		return PX_TRUE;
	case 1: // const_float - float bit-pattern
		*out = *(px_float*)&value;
		return PX_TRUE;
	case 2: // rx - interpret as float
		if (value > 7)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_get_float: rx index out of range");
			return PX_FALSE;
		}
		*out = *(px_float*)&pthread->r[value];
		return PX_TRUE;
	case 3: // fx
		if (value > 3)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_get_float: fx index out of range");
			return PX_FALSE;
		}
		*out = pthread->f[value];
		return PX_TRUE;
	case 4: // addr (global)
		if (value >= pmac->runtime_memory_size)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_get_float: addr out of range");
			return PX_FALSE;
		}
		*out = *(px_float*)&pmac->runtime_memory[value];
		return PX_TRUE;
	case 5: // bp-addr (local)
	{
		px_dword real_addr = pthread->bp - value;
		if (real_addr >= pmac->runtime_memory_size)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_get_float: bp-addr out of range");
			return PX_FALSE;
		}
		*out = *(px_float*)&pmac->runtime_memory[real_addr];
		return PX_TRUE;
	}
	default:
		PX_Machine_Crash(pmac, "PX_Machine_ee_get_float: unknown type");
		return PX_FALSE;
	}
}

// ee instruction helper: set integer operand value by type (type 0/1 = const, not writable)
// type: 0=const_int  1=const_float  2=rx  3=fx  4=addr(global)  5=bp-addr(local)
static px_bool PX_Machine_ee_set_int(PX_Machine* pmac, PX_Machine_Thread* pthread, px_byte type, px_dword value, px_dword src)
{
	switch (type)
	{
	case 0: // const_int - not writable
	case 1: // const_float - not writable
		PX_Machine_Crash(pmac, "PX_Machine_ee_set_int: cannot write to const");
		return PX_FALSE;
	case 2: // rx
		if (value > 7)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_set_int: rx index out of range");
			return PX_FALSE;
		}
		pthread->r[value] = src;
		return PX_TRUE;
	case 3: // fx - raw bits
		if (value > 3)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_set_int: fx index out of range");
			return PX_FALSE;
		}
		*(px_dword*)&pthread->f[value] = src;
		return PX_TRUE;
	case 4: // addr (global)
		if (value >= pmac->runtime_memory_size)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_set_int: addr out of range");
			return PX_FALSE;
		}
		*(px_dword*)&pmac->runtime_memory[value] = src;
		return PX_TRUE;
	case 5: // bp-addr (local)
	{
		px_dword real_addr = pthread->bp - value;
		if (real_addr >= pmac->runtime_memory_size)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_set_int: bp-addr out of range");
			return PX_FALSE;
		}
		*(px_dword*)&pmac->runtime_memory[real_addr] = src;
		return PX_TRUE;
	}
	default:
		PX_Machine_Crash(pmac, "PX_Machine_ee_set_int: unknown type");
		return PX_FALSE;
	}
}

// ee instruction helper: set float operand value by type (type 0/1 = const, not writable)
// type: 0=const_int  1=const_float  2=rx  3=fx  4=addr(global)  5=bp-addr(local)
static px_bool PX_Machine_ee_set_float(PX_Machine* pmac, PX_Machine_Thread* pthread, px_byte type, px_dword value, px_float src)
{
	switch (type)
	{
	case 0: // const_int - not writable
	case 1: // const_float - not writable
		PX_Machine_Crash(pmac, "PX_Machine_ee_set_float: cannot write to const");
		return PX_FALSE;
	case 2: // rx - interpret as float
		if (value > 7)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_set_float: rx index out of range");
			return PX_FALSE;
		}
		*(px_float*)&pthread->r[value] = src;
		return PX_TRUE;
	case 3: // fx
		if (value > 3)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_set_float: fx index out of range");
			return PX_FALSE;
		}
		pthread->f[value] = src;
		return PX_TRUE;
	case 4: // addr (global)
		if (value >= pmac->runtime_memory_size)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_set_float: addr out of range");
			return PX_FALSE;
		}
		*(px_float*)&pmac->runtime_memory[value] = src;
		return PX_TRUE;
	case 5: // bp-addr (local)
	{
		px_dword real_addr = pthread->bp - value;
		if (real_addr >= pmac->runtime_memory_size)
		{
			PX_Machine_Crash(pmac, "PX_Machine_ee_set_float: bp-addr out of range");
			return PX_FALSE;
		}
		*(px_float*)&pmac->runtime_memory[real_addr] = src;
		return PX_TRUE;
	}
	default:
		PX_Machine_Crash(pmac, "PX_Machine_ee_set_float: unknown type");
		return PX_FALSE;
	}
}

// ee_push: [opcode][type][value 4B] -- 6 bytes
// reads operand and pushes it onto the stack
px_void PX_Machine_Execute_Opcode_ee_push(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_byte type;
	px_dword value;
	px_dword operand;
	if (pthread->ip + 6 > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_push: thread ip out of range");
		return;
	}
	type = ip[1];
	value = *(px_dword*)(ip + 2);
	if (!PX_Machine_ee_get_int(pmac, pthread, type, value, &operand))
		return;
	pthread->sp -= 4;
	if (pthread->sp > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_push: stack overflow");
		return;
	}
	*(px_dword*)&pmac->runtime_memory[pthread->sp] = operand;
	pthread->ip += 6;
}

// ee_pop: [opcode][type][value 4B] -- 6 bytes
// pops from stack and writes to operand
px_void PX_Machine_Execute_Opcode_ee_pop(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_byte type;
	px_dword value;
	px_dword stack_val;
	if (pthread->ip + 6 > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_pop: thread ip out of range");
		return;
	}
	type = ip[1];
	value = *(px_dword*)(ip + 2);
	if (pthread->sp + 4 > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_pop: stack underflow");
		return;
	}
	stack_val = *(px_dword*)&pmac->runtime_memory[pthread->sp];
	pthread->sp += 4;
	if (!PX_Machine_ee_set_int(pmac, pthread, type, value, stack_val))
		return;
	pthread->ip += 6;
}

// ee_neg: [opcode][type][value 4B] -- 6 bytes
// reads operand (integer), negates (~v+1), writes back
px_void PX_Machine_Execute_Opcode_ee_neg(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_byte type;
	px_dword value;
	px_dword operand;
	if (pthread->ip + 6 > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_neg: thread ip out of range");
		return;
	}
	type = ip[1];
	value = *(px_dword*)(ip + 2);
	if (!PX_Machine_ee_get_int(pmac, pthread, type, value, &operand))
		return;
	operand = (~operand) + 1;
	if (!PX_Machine_ee_set_int(pmac, pthread, type, value, operand))
		return;
	pthread->ip += 6;
}

// ee_fneg: [opcode][type][value 4B] -- 6 bytes
// reads operand (float), negates (-v), writes back
px_void PX_Machine_Execute_Opcode_ee_fneg(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_byte type;
	px_dword value;
	px_float operand;
	if (pthread->ip + 6 > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_fneg: thread ip out of range");
		return;
	}
	type = ip[1];
	value = *(px_dword*)(ip + 2);
	if (!PX_Machine_ee_get_float(pmac, pthread, type, value, &operand))
		return;
	operand = -operand;
	if (!PX_Machine_ee_set_float(pmac, pthread, type, value, operand))
		return;
	pthread->ip += 6;
}

// ee_not: [opcode][type][value 4B] -- 6 bytes
// reads operand (integer), bitwise NOT (~v), writes back
px_void PX_Machine_Execute_Opcode_ee_not(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_byte type;
	px_dword value;
	px_dword operand;
	if (pthread->ip + 6 > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_not: thread ip out of range");
		return;
	}
	type = ip[1];
	value = *(px_dword*)(ip + 2);
	if (!PX_Machine_ee_get_int(pmac, pthread, type, value, &operand))
		return;
	operand = ~operand;
	if (!PX_Machine_ee_set_int(pmac, pthread, type, value, operand))
		return;
	pthread->ip += 6;
}

// ee_call: [opcode][type][value 4B] -- 6 bytes
// reads call address operand, push return_addr then push bp, jump
px_void PX_Machine_Execute_Opcode_ee_call(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_byte type;
	px_dword value;
	px_dword call_addr;
	if (pthread->ip + 6 > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_call: thread ip out of range");
		return;
	}
	type = ip[1];
	value = *(px_dword*)(ip + 2);
	if (!PX_Machine_ee_get_int(pmac, pthread, type, value, &call_addr))
		return;
	if (call_addr >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_call: call address out of range");
		return;
	}
	// push return address (ip + 6)
	pthread->sp -= 4;
	if (pthread->sp > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_call: stack overflow");
		return;
	}
	*(px_dword*)&pmac->runtime_memory[pthread->sp] = pthread->ip + 6;
	// push bp
	pthread->sp -= 4;
	if (pthread->sp > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_call: stack overflow");
		return;
	}
	*(px_dword*)&pmac->runtime_memory[pthread->sp] = pthread->bp;
	pthread->ip = call_addr;
}

// Internal helper for ee integer binary ops: [opcode][type_dst][type_src1][type_src2][dst 4B][src1 4B][src2 4B] -- 16 bytes
static px_void PX_Machine_Execute_Opcode_ee_int_binop(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip, px_int opcode)
{
	px_byte type_dst, type_src1, type_src2;
	px_dword val_dst, val_src1, val_src2;
	px_dword src1, src2, result;
	if (pthread->ip + 16 > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_int_binop: thread ip out of range");
		return;
	}
	type_dst  = ip[1];
	type_src1 = ip[2];
	type_src2 = ip[3];
	val_dst  = *(px_dword*)(ip + 4);
	val_src1 = *(px_dword*)(ip + 8);
	val_src2 = *(px_dword*)(ip + 12);
	if (!PX_Machine_ee_get_int(pmac, pthread, type_src1, val_src1, &src1))
		return;
	if (!PX_Machine_ee_get_int(pmac, pthread, type_src2, val_src2, &src2))
		return;
	switch (opcode)
	{
	case 0: result = src1 + src2; break;
	case 1: result = src1 - src2; break;
	case 2: result = src1 * src2; break;
	case 3:
		if (src2 == 0) { PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_div: divide by zero"); return; }
		result = src1 / src2;
		break;
	case 4:
		if ((px_int)src2 == 0) { PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_idiv: divide by zero"); return; }
		result = (px_dword)((px_int)src1 / (px_int)src2);
		break;
	case 5:
		if (src2 == 0) { PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_mod: divide by zero"); return; }
		result = src1 % src2;
		break;
	case 6: result = src1 & src2; break;
	case 7: result = src1 | src2; break;
	case 8: result = src1 ^ src2; break;
	case 9: result = src1 << src2; break;
	case 10: result = src1 >> src2; break;
	default:
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_int_binop: invalid opcode");
		return;
	}
	if (!PX_Machine_ee_set_int(pmac, pthread, type_dst, val_dst, result))
		return;
	pthread->ip += 16;
}

// Internal helper for ee float binary ops: [opcode][type_dst][type_src1][type_src2][dst 4B][src1 4B][src2 4B] -- 16 bytes
static px_void PX_Machine_Execute_Opcode_ee_float_binop(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip, px_int opcode)
{
	px_byte type_dst, type_src1, type_src2;
	px_dword val_dst, val_src1, val_src2;
	px_float src1, src2, result;
	if (pthread->ip + 16 > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_float_binop: thread ip out of range");
		return;
	}
	type_dst  = ip[1];
	type_src1 = ip[2];
	type_src2 = ip[3];
	val_dst  = *(px_dword*)(ip + 4);
	val_src1 = *(px_dword*)(ip + 8);
	val_src2 = *(px_dword*)(ip + 12);
	if (!PX_Machine_ee_get_float(pmac, pthread, type_src1, val_src1, &src1))
		return;
	if (!PX_Machine_ee_get_float(pmac, pthread, type_src2, val_src2, &src2))
		return;
	switch (opcode)
	{
	case 0: result = src1 + src2; break;
	case 1: result = src1 - src2; break;
	case 2: result = src1 * src2; break;
	case 3:
		if (src2 == 0) { PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_fdiv: divide by zero"); return; }
		result = src1 / src2;
		break;
	default:
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_float_binop: invalid opcode");
		return;
	}
	if (!PX_Machine_ee_set_float(pmac, pthread, type_dst, val_dst, result))
		return;
	pthread->ip += 16;
}

px_void PX_Machine_Execute_Opcode_ee_add(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	PX_Machine_Execute_Opcode_ee_int_binop(pmac, pthread, ip, 0);
}

px_void PX_Machine_Execute_Opcode_ee_sub(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	PX_Machine_Execute_Opcode_ee_int_binop(pmac, pthread, ip, 1);
}

px_void PX_Machine_Execute_Opcode_ee_mul(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	PX_Machine_Execute_Opcode_ee_int_binop(pmac, pthread, ip, 2);
}

px_void PX_Machine_Execute_Opcode_ee_div(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	PX_Machine_Execute_Opcode_ee_int_binop(pmac, pthread, ip, 3);
}

px_void PX_Machine_Execute_Opcode_ee_idiv(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	PX_Machine_Execute_Opcode_ee_int_binop(pmac, pthread, ip, 4);
}

px_void PX_Machine_Execute_Opcode_ee_mod(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	PX_Machine_Execute_Opcode_ee_int_binop(pmac, pthread, ip, 5);
}

px_void PX_Machine_Execute_Opcode_ee_and(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	PX_Machine_Execute_Opcode_ee_int_binop(pmac, pthread, ip, 6);
}

px_void PX_Machine_Execute_Opcode_ee_or(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	PX_Machine_Execute_Opcode_ee_int_binop(pmac, pthread, ip, 7);
}

px_void PX_Machine_Execute_Opcode_ee_xor(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	PX_Machine_Execute_Opcode_ee_int_binop(pmac, pthread, ip, 8);
}

px_void PX_Machine_Execute_Opcode_ee_shl(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	PX_Machine_Execute_Opcode_ee_int_binop(pmac, pthread, ip, 9);
}

px_void PX_Machine_Execute_Opcode_ee_shr(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	PX_Machine_Execute_Opcode_ee_int_binop(pmac, pthread, ip, 10);
}

px_void PX_Machine_Execute_Opcode_ee_fadd(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	PX_Machine_Execute_Opcode_ee_float_binop(pmac, pthread, ip, 0);
}

px_void PX_Machine_Execute_Opcode_ee_fsub(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	PX_Machine_Execute_Opcode_ee_float_binop(pmac, pthread, ip, 1);
}

px_void PX_Machine_Execute_Opcode_ee_fmul(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	PX_Machine_Execute_Opcode_ee_float_binop(pmac, pthread, ip, 2);
}

px_void PX_Machine_Execute_Opcode_ee_fdiv(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	PX_Machine_Execute_Opcode_ee_float_binop(pmac, pthread, ip, 3);
}

// ee_cmp: [opcode][type_src1][type_src2][pad 1B][src1 4B][src2 4B] -- 12 bytes
// sets Z/C/N/V flags the same as the existing cmp instruction
px_void PX_Machine_Execute_Opcode_ee_cmp(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_byte type_src1, type_src2;
	px_dword val_src1, val_src2;
	px_dword src1, src2;
	if (pthread->ip + 12 > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_cmp: thread ip out of range");
		return;
	}
	type_src1 = ip[1];
	type_src2 = ip[2];
	// ip[3] is padding byte
	val_src1 = *(px_dword*)(ip + 4);
	val_src2 = *(px_dword*)(ip + 8);
	if (!PX_Machine_ee_get_int(pmac, pthread, type_src1, val_src1, &src1))
		return;
	if (!PX_Machine_ee_get_int(pmac, pthread, type_src2, val_src2, &src2))
		return;
	pthread->Z = (px_bool)(src1 == src2 ? PX_TRUE : PX_FALSE);
	pthread->C = (px_bool)(src1 < src2 ? PX_TRUE : PX_FALSE);
	pthread->N = ((src1 - src2) & 0x80000000) ? PX_TRUE : PX_FALSE;
	pthread->V = (src1 & 0x80000000) != (src2 & 0x80000000) \
		&& (src1 & 0x80000000) != ((src1 - src2) & 0x80000000) ? \
		PX_TRUE : PX_FALSE;
	pthread->ip += 12;
}

// ee_fcmp: [opcode][type_src1][type_src2][pad 1B][src1 4B][src2 4B] -- 12 bytes
// float compare: sets fflag (fZ/fC) then copies to integer flags
px_void PX_Machine_Execute_Opcode_ee_fcmp(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	px_byte type_src1, type_src2;
	px_dword val_src1, val_src2;
	px_float src1, src2;
	if (pthread->ip + 12 > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_ee_fcmp: thread ip out of range");
		return;
	}
	type_src1 = ip[1];
	type_src2 = ip[2];
	// ip[3] is padding byte
	val_src1 = *(px_dword*)(ip + 4);
	val_src2 = *(px_dword*)(ip + 8);
	if (!PX_Machine_ee_get_float(pmac, pthread, type_src1, val_src1, &src1))
		return;
	if (!PX_Machine_ee_get_float(pmac, pthread, type_src2, val_src2, &src2))
		return;
	pthread->Z = (px_bool)(src1 == src2 ? PX_TRUE : PX_FALSE);
	pthread->C = (px_bool)(src1 < src2 ? PX_TRUE : PX_FALSE);
	// copy float flags to integer flags (same as ff2f)
	pthread->N = PX_FALSE;
	pthread->V = PX_FALSE;
	pthread->ip += 12;
}

px_void PX_Machine_Execute_Opcode_push(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	if (pthread->ip + 2 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_push: thread ip out of range");
		return;
	}
	pthread->sp -= 4;
	if (pthread->sp > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_push: stack overflow");
		return;
	}
	switch (ip[1] & 0x0f)
	{
	case 1:
	{
		px_int register_index = (ip[1] & 0xf0) >> 4;
		if (register_index <= 7)
		{
			*(px_dword*)&pmac->runtime_memory[pthread->sp] = pthread->r[register_index];
		}
		else
		{
			PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_push: cannot push flag register");
			return;
		}
	}
	break;
	default:
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_push: push source error");
		return;
	}
	pthread->ip += 2;
}

px_void PX_Machine_Execute_Opcode_pop(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	if (pthread->ip + 2 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_pop: thread ip out of range");
		return;
	}
	switch (ip[1] & 0x0f)
	{
	case 0:
	{
		//do nothing
	}
	break;
	case 1:
	{
		px_int register_index = (ip[1] & 0xf0) >> 4;
		if (register_index<=7)
		{
			if (register_index== 5)
			{
				PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_pop: cannot pop to sp register");
				return;
			}
			pthread->r[register_index] = *(px_dword*)&pmac->runtime_memory[pthread->sp];
		}
		else
		{
			PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_pop: cannot pop to flag register");
			return;
		}
		
	}
	break;
	default:
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_pop: pop target error");
		return;
	}
	pthread->sp += 4;
	if (pthread->sp > pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_popn: stack pointer out of range");
		return;
	}
	pthread->ip += 2;
}

px_void PX_Machine_Execute_Opcode_popn(PX_Machine* pmac, PX_Machine_Thread* pthread, const px_byte* ip)
{
	if (pthread->ip + 1 >= pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_pop: thread ip out of range");
		return;
	}
	pthread->sp += pthread->r[0];
	if (pthread->sp>=pmac->runtime_memory_size)
	{
		PX_Machine_Crash(pmac, "PX_Machine_Execute_Opcode_popn: stack pointer out of range");
		return;
	}
	pthread->ip += 1;
}


px_void PX_Machin_OnPayload(PX_Machine* pmac, px_abi *payload_abi)
{
	//process payload

}

px_void PX_Machine_Run_Thread_ISA(PX_Machine* pmac, PX_Machine_Thread* pthread,px_byte opcode)
{
	switch (opcode)
	{
	case PX_SYNTAX_MACHINE_OPCODE_TRAP:
		pthread->state = PX_MACHINE_STATE_PAUSE;
		break;
	case PX_SYNTAX_MACHINE_OPCODE_NOP:
		pthread->ip += 1;
		break;
	case PX_SYNTAX_MACHINE_OPCODE_MOVR:
		PX_Machine_Execute_Opcode_movr(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_MOVF:
	case PX_SYNTAX_MACHINE_OPCODE_MOVC:
		PX_Machine_Execute_Opcode_movc_movf(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_MOVN:
		PX_Machine_Execute_Opcode_movn(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_F2I:
		PX_Machine_Execute_Opcode_f2i(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_F2U:
		PX_Machine_Execute_Opcode_f2u(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_I2F:
		PX_Machine_Execute_Opcode_i2f(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_U2F:
		PX_Machine_Execute_Opcode_u2f(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_FF2F:
		PX_Machine_Execute_Opcode_ff2f(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_LOADU8:
		PX_Machine_Execute_Opcode_loadxx(pmac, pthread, &pmac->runtime_memory[pthread->ip], PX_FALSE, 8);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_LOADU16:
		PX_Machine_Execute_Opcode_loadxx(pmac, pthread, &pmac->runtime_memory[pthread->ip], PX_FALSE, 16);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_LOADU32:
		PX_Machine_Execute_Opcode_loadxx(pmac, pthread, &pmac->runtime_memory[pthread->ip], PX_FALSE, 32);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_LOADI8:
		PX_Machine_Execute_Opcode_loadxx(pmac, pthread, &pmac->runtime_memory[pthread->ip], PX_TRUE, 8);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_LOADI16:
		PX_Machine_Execute_Opcode_loadxx(pmac, pthread, &pmac->runtime_memory[pthread->ip], PX_TRUE, 16);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_LOADI32:
		PX_Machine_Execute_Opcode_loadxx(pmac, pthread, &pmac->runtime_memory[pthread->ip], PX_TRUE, 32);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_STORE8:
		PX_Machine_Execute_Opcode_storex(pmac, pthread, &pmac->runtime_memory[pthread->ip], 8);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_STORE16:
		PX_Machine_Execute_Opcode_storex(pmac, pthread, &pmac->runtime_memory[pthread->ip], 16);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_STORE32:
		PX_Machine_Execute_Opcode_storex(pmac, pthread, &pmac->runtime_memory[pthread->ip], 32);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_PUSH:
		PX_Machine_Execute_Opcode_push(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_PUSHAD:
		PX_Machine_Execute_Opcode_pushad(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_POP:
		PX_Machine_Execute_Opcode_pop(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_POPAD:
		PX_Machine_Execute_Opcode_popad(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_POPN:
		PX_Machine_Execute_Opcode_popn(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_ADD:
		PX_Machine_Execute_Opcode_add(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_SUB:
		PX_Machine_Execute_Opcode_sub(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_MUL:
		PX_Machine_Execute_Opcode_mul(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_DIV:
		PX_Machine_Execute_Opcode_div(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_IDIV:
		PX_Machine_Execute_Opcode_idiv(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_MOD:
		PX_Machine_Execute_Opcode_mod(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_NEG:
		PX_Machine_Execute_Opcode_neg(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_AND:
		PX_Machine_Execute_Opcode_and(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_OR:
		PX_Machine_Execute_Opcode_or(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_XOR:
		PX_Machine_Execute_Opcode_xor(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_SHL:
		PX_Machine_Execute_Opcode_shl(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_SHR:
		PX_Machine_Execute_Opcode_shr(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_NOT:
		PX_Machine_Execute_Opcode_not(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_FADD:
		PX_Machine_Execute_Opcode_fadd(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_FSUB:
		PX_Machine_Execute_Opcode_fsub(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_FMUL:
		PX_Machine_Execute_Opcode_fmul(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_FDIV:
		PX_Machine_Execute_Opcode_fdiv(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_FNEG:
		PX_Machine_Execute_Opcode_fneg(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_CMP:
		PX_Machine_Execute_Opcode_cmp(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_JMP:
		PX_Machine_Execute_Opcode_jmp(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_JE:
		PX_Machine_Execute_Opcode_je(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_JNE:
		PX_Machine_Execute_Opcode_jne(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_JL:
		PX_Machine_Execute_Opcode_jl(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_JLE:
		PX_Machine_Execute_Opcode_jle(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_JG:
		PX_Machine_Execute_Opcode_jg(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_JGE:
		PX_Machine_Execute_Opcode_jge(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_CALL:
		PX_Machine_Execute_Opcode_call(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_CALLR:
		PX_Machine_Execute_Opcode_callr(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_JMPR:
		PX_Machine_Execute_Opcode_jmpr(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_ADDSP:
		PX_Machine_Execute_Opcode_addsp(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_SUBSP:
		PX_Machine_Execute_Opcode_subsp(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_FCMP:
		PX_Machine_Execute_Opcode_fcmp(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_FCOMI:
		PX_Machine_Execute_Opcode_fcomi(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_RET:
		PX_Machine_Execute_Opcode_ret(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_ADD:
		PX_Machine_Execute_Opcode_ee_add(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_SUB:
		PX_Machine_Execute_Opcode_ee_sub(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_MUL:
		PX_Machine_Execute_Opcode_ee_mul(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_DIV:
		PX_Machine_Execute_Opcode_ee_div(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_FADD:
		PX_Machine_Execute_Opcode_ee_fadd(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_FSUB:
		PX_Machine_Execute_Opcode_ee_fsub(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_FMUL:
		PX_Machine_Execute_Opcode_ee_fmul(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_FDIV:
		PX_Machine_Execute_Opcode_ee_fdiv(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_IDIV:
		PX_Machine_Execute_Opcode_ee_idiv(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_AND:
		PX_Machine_Execute_Opcode_ee_and(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_NOT:
		PX_Machine_Execute_Opcode_ee_not(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_OR:
		PX_Machine_Execute_Opcode_ee_or(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_POP:
		PX_Machine_Execute_Opcode_ee_pop(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_PUSH:
		PX_Machine_Execute_Opcode_ee_push(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_SHL:
		PX_Machine_Execute_Opcode_ee_shl(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_SHR:
		PX_Machine_Execute_Opcode_ee_shr(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_XOR:
		PX_Machine_Execute_Opcode_ee_xor(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_CALL:
		PX_Machine_Execute_Opcode_ee_call(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_MOD:
		PX_Machine_Execute_Opcode_ee_mod(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_NEG:
		PX_Machine_Execute_Opcode_ee_neg(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_FNEG:
		PX_Machine_Execute_Opcode_ee_fneg(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_CMP:
		PX_Machine_Execute_Opcode_ee_cmp(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_FCMP:
		PX_Machine_Execute_Opcode_ee_fcmp(pmac, pthread, &pmac->runtime_memory[pthread->ip]);
		break;
	default:
	{
		px_char crash_message[256];
		PX_sprintf2(crash_message, sizeof(crash_message), "PX_Machine_Update_Thread: unknown opcode %1 at ip=%2", PX_STRINGFORMAT_INT(opcode), PX_STRINGFORMAT_INT(pthread->ip));
		PX_Machine_Crash(pmac, crash_message);
		return;
	}
	}

}


px_void PX_Machine_Update_Thread(PX_Machine* pmac,px_int thread_index, px_int tick, px_dword elapsed)
{
	PX_Machine_Thread* pthread;
	pthread = PX_VECTORAT(PX_Machine_Thread, &pmac->threads, thread_index);

	while (PX_TRUE)
	{
		if (pthread->sleep)
		{
			if (pthread->sleep <= elapsed)
			{
				pthread->sleep = 0;
			}
			else
			{
				pthread->sleep -= elapsed;
				return;
			}
		}

		if (pthread->ip >= pmac->runtime_memory_size)
		{
			px_char crash_message[256];
			PX_sprintf2(crash_message, sizeof(crash_message), "PX_Machine_Update_Thread: thread ip out of range ip=%1 size=%2", PX_STRINGFORMAT_INT(pthread->ip), PX_STRINGFORMAT_INT(pmac->runtime_memory_size));
			PX_Machine_Crash(pmac, crash_message);
			return;
		}

		//step flag
		if (pthread->T)
		{
			pthread->T = 0;
			tick = 1;
		}

		if (tick==0)
		{
			break;
		}

		switch (pthread->state)
		{
		case PX_MACHINE_STATE_IDLE:
			return;
		case PX_MACHINE_STATE_RUNNING:
		{
			px_byte opcode;
			opcode = pmac->runtime_memory[pthread->ip];
			PX_Machine_Run_Thread_ISA(pmac, pthread, opcode);
			if (tick > 0)tick--;
		}
		break;
		case PX_MACHINE_STATE_PAUSE:
			return;
		case PX_MACHINE_STATE_ERROR:
			return;
		}
	}
}

px_void PX_Machine_Update(PX_Machine* pmac, px_int tick, px_dword elapsed)
{
	switch (pmac->state)
	{
	case PX_MACHINE_STATE_IDLE:
	{
		//do nothing
	}
	break;
	case PX_MACHINE_STATE_RUNNING:
	{
		PX_Machine_Thread* pthread;
		if (!PX_VECTOR_IN_RANGE(&pmac->threads, pmac->T))
		{
			PX_Machine_Crash(pmac, "PX_Machine_Update: current thread index out of range");
			return;
		}
		pthread = PX_VECTORAT(PX_Machine_Thread, &pmac->threads, pmac->T);
		PX_Machine_Update_Thread(pmac, pmac->T, tick, elapsed);

	}
	break;
	case PX_MACHINE_STATE_PAUSE:
	{
		//do nothing
	}
	break;
	case PX_MACHINE_STATE_ERROR:
	{
		//do nothing
	}
	break;
	}
	
}

px_bool PX_Machine_LoadBin(PX_Machine* pMachine, const px_byte* bin, px_int size)
{
	return PX_FALSE;
}

const px_char* PX_Machine_Start(PX_Machine* pMachine)
{
	if (pMachine->state== PX_MACHINE_STATE_IDLE)
	{
		pMachine->state = PX_MACHINE_STATE_RUNNING;
		return "ok";
	}
	else
	{
		PX_Machine_Message(pMachine, "PX_Machine_Start: machine not in idle state");
		return "PX_Machine_Start: machine not in idle state";
	}
	
}

const px_char* PX_Machine_Pause(PX_Machine* pMachine)
{
	if (pMachine->state == PX_MACHINE_STATE_RUNNING)
	{
		pMachine->state = PX_MACHINE_STATE_PAUSE;
		return "ok";
	}
	else
	{
		PX_Machine_Message(pMachine, "PX_Machine_Pause: machine not in running state");
		return "PX_Machine_Pause: machine not in running state";
	}
}

const px_char* PX_Machine_Reset(PX_Machine* pMachine)
{
	pMachine->state = PX_MACHINE_STATE_IDLE;
	if (pMachine->message.buffer)
		PX_StringClear(&pMachine->message);
	//clear threads
	PX_VectorClear(&pMachine->threads);
	/*
	//clear runtime memory
	if (pMachine->mp && pMachine->runtime_memory)
	{
		MP_Free(pMachine->mp, pMachine->runtime_memory);
		pMachine->runtime_memory = PX_NULL;
		pMachine->runtime_memory_size = 0;
	}
	*/
	return "ok";
}

const px_char* PX_Machine_Restart(PX_Machine* pMachine)
{
	PX_Machine_Reset(pMachine);
	PX_Machine_Start(pMachine);
	return "ok";
}

px_bool PX_Machine_Initialize(px_memorypool* mp, PX_Machine* pmac)
{
	PX_memset(pmac, 0, sizeof(PX_Machine));
	pmac->mp = mp;
	pmac->state = PX_MACHINE_STATE_IDLE;
	pmac->thread_stack_size = PX_MACHINE_DEFAULT_THREAD_STACK_SIZE;
	PX_memset(pmac->ip_breakpoint, 0xFF, sizeof(pmac->ip_breakpoint));
	if (!PX_StringInitialize(mp, &pmac->message))
	{
		return PX_FALSE;
	}
	if (!PX_VectorInitialize(mp, &pmac->threads, sizeof(PX_Machine_Thread), 1))
	{
		PX_StringFree(&pmac->message);
		return PX_FALSE;
	}
	return PX_TRUE;
}

px_void PX_Machine_Free(PX_Machine* pmac)
{
	PX_Machine_Reset(pmac);

	if(pmac->mp && pmac->runtime_memory)
		MP_Free(pmac->mp,pmac->runtime_memory);

	pmac->runtime_memory = PX_NULL;
	pmac->runtime_memory_size = 0;
	PX_VectorFree(&pmac->threads);
	PX_StringFree(&pmac->message);


}

px_void PX_Machine_Message(PX_Machine* pMachine, const px_char* message)
{
	if (!PX_StringCat(&pMachine->message, message))
	{
		PX_StringClear(&pMachine->message);
		PX_StringCat(&pMachine->message, "clear message");
	}
}

px_string* PX_Machine_GetMessage(PX_Machine* pMachine)
{
	return &pMachine->message;
}



static px_bool PX_Machine_Execute_Opcode_get_state(PX_Machine* pMachine, px_abi* ppayload_abi, px_abi* preturn_abi)
{
	const px_char* state_str;
	switch (pMachine->state)
	{
	case PX_MACHINE_STATE_IDLE:    state_str = "idle"; break;
	case PX_MACHINE_STATE_RUNNING: state_str = "running"; break;
	case PX_MACHINE_STATE_PAUSE:   state_str = "pause"; break;
	case PX_MACHINE_STATE_ERROR:   state_str = "error"; break;
	default:                       state_str = "error"; break;
	}
	if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
		return PX_FALSE;
	if (!PX_AbiSet_string(preturn_abi, "state", state_str))
		return PX_FALSE;

	if (pMachine->state != PX_MACHINE_STATE_IDLE)
	{
		if (!PX_AbiSet_data(preturn_abi, "bp", pMachine->ip_breakpoint, (px_int)sizeof(pMachine->ip_breakpoint)))
			return PX_FALSE;

		if (pMachine->T >= 0 && pMachine->T < PX_VectorSize(&pMachine->threads))
		{
			PX_Machine_Thread* pThread = PX_VECTORAT(PX_Machine_Thread, &pMachine->threads, pMachine->T);
			if (!PX_AbiSet_data(preturn_abi, "r", pThread->r, sizeof(pThread->r)))
				return PX_FALSE;
			if (!PX_AbiSet_data(preturn_abi, "f", pThread->f, sizeof(pThread->f)))
				return PX_FALSE;
		}
	}
	return PX_TRUE;
}

static px_bool PX_Machine_Execute_Opcode_load(PX_Machine* pMachine, px_abi* ppayload_abi, px_abi* preturn_abi)
{
	px_abi bin_abi;
	px_dword text_size = 0, rdata_size = 0;
	const px_byte* ptext;
	const px_byte* prdata;
	px_uint load_size;
	px_uint stack_size;
	px_uint required_memory_size;
	px_byte* new_runtime_memory;
	PX_Machine_Thread main_thread;

	if (!PX_AbiGet_AbiReadOnly(ppayload_abi, &bin_abi, "bin"))
		return PX_AbiSet_string(preturn_abi, "return", "load: missing bin abi");
	if (!PX_AbiCheck(&bin_abi))
		return PX_AbiSet_string(preturn_abi, "return", "load: invalid bin abi");

	ptext = (const px_byte*)PX_AbiGet_data(&bin_abi, "text", &text_size);
	if (!ptext || !text_size)
		return PX_AbiSet_string(preturn_abi, "return", "load: missing text segment");

	// rdata segment is optional
	prdata = (const px_byte*)PX_AbiGet_data(&bin_abi, "rdata", &rdata_size);
	
	if (text_size > (px_dword)0x7fffffff || rdata_size > (px_dword)0x7fffffff - text_size)
		return PX_AbiSet_string(preturn_abi, "return", "load: bin segment too large");

	load_size = (px_uint)(text_size + rdata_size);
	stack_size = pMachine->thread_stack_size ? pMachine->thread_stack_size : PX_MACHINE_DEFAULT_THREAD_STACK_SIZE;
	if (stack_size > (px_uint)0x7fffffff - load_size)
		return PX_AbiSet_string(preturn_abi, "return", "load: runtime memory size overflow");

	required_memory_size = load_size + stack_size;
	if (required_memory_size < PX_MACHINE_DEFAULT_RUNTIME_MEMORY_SIZE)
		required_memory_size = PX_MACHINE_DEFAULT_RUNTIME_MEMORY_SIZE;
	if (pMachine->runtime_memory_size > required_memory_size)
		required_memory_size = pMachine->runtime_memory_size;

	new_runtime_memory = (px_byte*)MP_Malloc(pMachine->mp, required_memory_size);
	if (!new_runtime_memory)
		return PX_AbiSet_string(preturn_abi, "return", "load: alloc runtime memory failed");
	PX_memset(new_runtime_memory, 0, required_memory_size);
	PX_memcpy(new_runtime_memory, ptext, text_size);
	if (prdata && rdata_size)
		PX_memcpy(new_runtime_memory + text_size, prdata, rdata_size);

	PX_memset(&main_thread, 0, sizeof(main_thread));
	main_thread.state = PX_MACHINE_STATE_PAUSE;
	main_thread.ip = 0;
	main_thread.sp = required_memory_size;
	main_thread.bp = required_memory_size;
	main_thread.module_base = 0;

	
	PX_VectorClear(&pMachine->threads);
	if (!PX_VectorPushback(&pMachine->threads, &main_thread))
	{
		MP_Free(pMachine->mp, new_runtime_memory);
		return PX_AbiSet_string(preturn_abi, "return", "load: create main thread failed");
	}

	//free old runtime memory
	if (pMachine->runtime_memory)
		MP_Free(pMachine->mp, pMachine->runtime_memory);

	pMachine->runtime_memory = new_runtime_memory;
	pMachine->runtime_memory_size = required_memory_size;
	pMachine->thread_stack_size = stack_size;
	pMachine->T = 0;
	pMachine->state = PX_MACHINE_STATE_PAUSE;
	if (pMachine->message.buffer)
		PX_StringClear(&pMachine->message);

	if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
		return PX_FALSE;
	return PX_TRUE;
}

static px_bool PX_Machine_Execute_Opcode_run(PX_Machine* pMachine, px_abi* ppayload_abi, px_abi* preturn_abi)
{
	if (pMachine->state != PX_MACHINE_STATE_PAUSE)
		return PX_AbiSet_string(preturn_abi, "return", "run: machine not in pause state");

	pMachine->state = PX_MACHINE_STATE_RUNNING;
	if (PX_VECTOR_IN_RANGE(&pMachine->threads, pMachine->T))
	{
		PX_Machine_Thread* pthread = PX_VECTORAT(PX_Machine_Thread, &pMachine->threads, pMachine->T);
		pthread->state = PX_MACHINE_STATE_RUNNING;
	}

	if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
		return PX_FALSE;
	return PX_TRUE;
}

static px_bool PX_Machine_Execute_Opcode_pause(PX_Machine* pMachine, px_abi* ppayload_abi, px_abi* preturn_abi)
{
	if (pMachine->state != PX_MACHINE_STATE_RUNNING)
		return PX_AbiSet_string(preturn_abi, "return", "pause: machine not in running state");

	pMachine->state = PX_MACHINE_STATE_PAUSE;
	if (PX_VECTOR_IN_RANGE(&pMachine->threads, pMachine->T))
	{
		PX_Machine_Thread* pthread = PX_VECTORAT(PX_Machine_Thread, &pMachine->threads, pMachine->T);
		pthread->state = PX_MACHINE_STATE_PAUSE;
	}

	if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
		return PX_FALSE;
	return PX_TRUE;
}

static px_bool PX_Machine_Execute_Opcode_reset(PX_Machine* pMachine, px_abi* ppayload_abi, px_abi* preturn_abi)
{
	PX_Machine_Reset(pMachine);
	if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
		return PX_FALSE;
	return PX_TRUE;
}

static px_bool PX_Machine_Execute_Opcode_stop(PX_Machine* pMachine, px_abi* ppayload_abi, px_abi* preturn_abi)
{
	pMachine->state = PX_MACHINE_STATE_IDLE;
	if (PX_VECTOR_IN_RANGE(&pMachine->threads, pMachine->T))
	{
		PX_Machine_Thread* pthread = PX_VECTORAT(PX_Machine_Thread, &pMachine->threads, pMachine->T);
		pthread->state = PX_MACHINE_STATE_IDLE;
	}

	if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
		return PX_FALSE;
	return PX_TRUE;
}

static px_bool PX_Machine_Execute_Opcode_set_register(PX_Machine* pMachine, px_abi* ppayload_abi, px_abi* preturn_abi)
{
	const px_char* reg_name;
	px_dword* pvalue;
	PX_Machine_Thread* pthread;

	if (pMachine->state == PX_MACHINE_STATE_IDLE)
		return PX_AbiSet_string(preturn_abi, "return", "set_register: no program loaded");

	if (!PX_VECTOR_IN_RANGE(&pMachine->threads, pMachine->T))
		return PX_AbiSet_string(preturn_abi, "return", "set_register: thread error");

	reg_name = PX_AbiGet_string(ppayload_abi, "register");
	if (!reg_name)
		return PX_AbiSet_string(preturn_abi, "return", "set_register: missing register name");

	pvalue = PX_AbiGet_dword(ppayload_abi, "value");
	if (!pvalue)
		return PX_AbiSet_string(preturn_abi, "return", "set_register: missing value");

	pthread = PX_VECTORAT(PX_Machine_Thread, &pMachine->threads, pMachine->T);

	if (PX_strequ(reg_name, "r0")) pthread->r0 = *pvalue;
	else if (PX_strequ(reg_name, "r1")) pthread->r1 = *pvalue;
	else if (PX_strequ(reg_name, "r2")) pthread->r2 = *pvalue;
	else if (PX_strequ(reg_name, "r3")) pthread->r3 = *pvalue;
	else if (PX_strequ(reg_name, "ip")) pthread->ip = *pvalue;
	else if (PX_strequ(reg_name, "sp")) pthread->sp = *pvalue;
	else if (PX_strequ(reg_name, "bp")) pthread->bp = *pvalue;
	else if (PX_strequ(reg_name, "flag")) pthread->flag = *pvalue;
	else
		return PX_AbiSet_string(preturn_abi, "return", "set_register: unknown register");

	if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
		return PX_FALSE;
	return PX_TRUE;
}

static px_bool PX_Machine_Execute_Opcode_set_memorysize(PX_Machine* pMachine, px_abi* ppayload_abi, px_abi* preturn_abi)
{
	px_dword* psize;

	if (pMachine->state != PX_MACHINE_STATE_IDLE)
		return PX_AbiSet_string(preturn_abi, "return", "set_memorysize: machine must be idle");

	psize = PX_AbiGet_dword(ppayload_abi, "size");
	if (!psize)
		return PX_AbiSet_string(preturn_abi, "return", "set_memorysize: missing size");

	pMachine->runtime_memory_size = *psize;

	if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
		return PX_FALSE;
	return PX_TRUE;
}

static px_bool PX_Machine_Execute_Opcode_set_stacksize(PX_Machine* pMachine, px_abi* ppayload_abi, px_abi* preturn_abi)
{
	px_dword* psize;

	if (pMachine->state != PX_MACHINE_STATE_IDLE)
		return PX_AbiSet_string(preturn_abi, "return", "set_stacksize: machine must be idle");

	psize = PX_AbiGet_dword(ppayload_abi, "size");
	if (!psize)
		return PX_AbiSet_string(preturn_abi, "return", "set_stacksize: missing size");

	pMachine->thread_stack_size = *psize;

	if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
		return PX_FALSE;
	return PX_TRUE;
}

static px_bool PX_Machine_Execute_Opcode_read_memory(PX_Machine* pMachine, px_abi* ppayload_abi, px_abi* preturn_abi)
{
	px_dword* paddress;
	px_dword* psize;

	if (pMachine->state == PX_MACHINE_STATE_IDLE || !pMachine->runtime_memory)
		return PX_AbiSet_string(preturn_abi, "return", "read_memory: no program loaded");

	paddress = PX_AbiGet_dword(ppayload_abi, "address");
	if (!paddress)
		return PX_AbiSet_string(preturn_abi, "return", "read_memory: missing address");

	psize = PX_AbiGet_dword(ppayload_abi, "size");
	if (!psize)
		return PX_AbiSet_string(preturn_abi, "return", "read_memory: missing size");

	if (*paddress + *psize > pMachine->runtime_memory_size || *paddress + *psize < *paddress)
		return PX_AbiSet_string(preturn_abi, "return", "read_memory: address out of range");

	if (!PX_AbiSet_data(preturn_abi, "data", pMachine->runtime_memory + *paddress, (px_int)*psize))
		return PX_FALSE;

	if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
		return PX_FALSE;
	return PX_TRUE;
}

static px_bool PX_Machine_Execute_Opcode_write_memory(PX_Machine* pMachine, px_abi* ppayload_abi, px_abi* preturn_abi)
{
	px_dword* paddress;
	px_dword data_size = 0;
	const px_byte* pdata;

	if (pMachine->state == PX_MACHINE_STATE_IDLE || !pMachine->runtime_memory)
		return PX_AbiSet_string(preturn_abi, "return", "write_memory: no program loaded");

	paddress = PX_AbiGet_dword(ppayload_abi, "address");
	if (!paddress)
		return PX_AbiSet_string(preturn_abi, "return", "write_memory: missing address");

	pdata = (const px_byte*)PX_AbiGet_data(ppayload_abi, "data", &data_size);
	if (!pdata || !data_size)
		return PX_AbiSet_string(preturn_abi, "return", "write_memory: missing data");

	if (*paddress + data_size > pMachine->runtime_memory_size || *paddress + data_size < *paddress)
		return PX_AbiSet_string(preturn_abi, "return", "write_memory: address out of range");

	PX_memcpy(pMachine->runtime_memory + *paddress, pdata, data_size);

	if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
		return PX_FALSE;
	return PX_TRUE;
}

static px_bool PX_Machine_Execute_Opcode_breakpoint(PX_Machine* pMachine, px_abi* ppayload_abi, px_abi* preturn_abi)
{
	px_dword* pip;
	px_int i;

	pip = PX_AbiGet_dword(ppayload_abi, "ip");
	if (!pip)
		return PX_AbiSet_string(preturn_abi, "return", "breakpoint: missing ip");

	for (i = 0; i < 32; i++)
	{
		if (pMachine->ip_breakpoint[i] == *pip)
		{
			pMachine->ip_breakpoint[i] = 0xFFFFFFFF;
			if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
				return PX_FALSE;
			return PX_TRUE;
		}
	}

	for (i = 0; i < 32; i++)
	{
		if (pMachine->ip_breakpoint[i] == 0xFFFFFFFF)
		{
			pMachine->ip_breakpoint[i] = *pip;
			if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
				return PX_FALSE;
			return PX_TRUE;
		}
	}

	return PX_AbiSet_string(preturn_abi, "return", "breakpoint: no free slot");
}

static px_bool PX_Machine_Execute_Opcode_memory_breakpoint(PX_Machine* pMachine, px_abi* ppayload_abi, px_abi* preturn_abi)
{
	px_dword* paddress;
	px_dword* psize;
	px_dword* pmask;
	px_int i;

	paddress = PX_AbiGet_dword(ppayload_abi, "address");
	if (!paddress)
		return PX_AbiSet_string(preturn_abi, "return", "memory_breakpoint: missing address");

	psize = PX_AbiGet_dword(ppayload_abi, "size");
	if (!psize)
		return PX_AbiSet_string(preturn_abi, "return", "memory_breakpoint: missing size");

	pmask = PX_AbiGet_dword(ppayload_abi, "mask");
	if (!pmask)
		return PX_AbiSet_string(preturn_abi, "return", "memory_breakpoint: missing mask");

	for (i = 0; i < 2; i++)
	{
		if (pMachine->memory_breakpoint_size[i] == 0)
		{
			pMachine->memory_breakpoint[i] = *paddress;
			pMachine->memory_breakpoint_size[i] = *psize;
			pMachine->memory_breakpoint_mask[i] = (px_byte)*pmask;
			if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
				return PX_FALSE;
			return PX_TRUE;
		}
	}

	return PX_AbiSet_string(preturn_abi, "return", "memory_breakpoint: no free slot");
}

static px_bool PX_Machine_Execute_Opcode_ucall(PX_Machine* pMachine, px_abi* ppayload_abi, px_abi* preturn_abi)
{
	px_dword* paddress;
	PX_Machine_Thread* pthread;

	if (pMachine->state == PX_MACHINE_STATE_IDLE)
		return PX_AbiSet_string(preturn_abi, "return", "call: no program loaded");

	if (!PX_VECTOR_IN_RANGE(&pMachine->threads, pMachine->T))
		return PX_AbiSet_string(preturn_abi, "return", "call: thread error");

	paddress = PX_AbiGet_dword(ppayload_abi, "address");
	if (!paddress)
		return PX_AbiSet_string(preturn_abi, "return", "call: missing address");

	if (*paddress >= pMachine->runtime_memory_size)
		return PX_AbiSet_string(preturn_abi, "return", "call: address out of range");

	pthread = PX_VECTORAT(PX_Machine_Thread, &pMachine->threads, pMachine->T);
	pthread->ip = *paddress;

	if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
		return PX_FALSE;
	return PX_TRUE;
}

static px_bool PX_Machine_Execute_Opcode_step(PX_Machine* pMachine, px_abi* ppayload_abi, px_abi* preturn_abi)
{
	PX_Machine_Thread* pthread;
	px_dword *ptick;
	px_int tick;

	if (pMachine->state != PX_MACHINE_STATE_PAUSE)
		return PX_AbiSet_string(preturn_abi, "return", "step: machine not in pause state");

	if (!PX_VECTOR_IN_RANGE(&pMachine->threads, pMachine->T))
		return PX_AbiSet_string(preturn_abi, "return", "step: VM thread error");

	ptick = PX_AbiGet_dword(ppayload_abi, "tick");
	if (!ptick)
		return PX_AbiSet_string(preturn_abi, "return", "step: missing tick");
	tick = (px_int)*ptick;
	if (tick <= 0)
		return PX_AbiSet_string(preturn_abi, "return", "step: invalid tick");

	pthread = PX_VECTORAT(PX_Machine_Thread, &pMachine->threads, pMachine->T);
	pthread->state = PX_MACHINE_STATE_RUNNING;
	pMachine->state = PX_MACHINE_STATE_RUNNING;

	PX_Machine_Update_Thread(pMachine, pMachine->T, tick, 0);

	pthread->state = PX_MACHINE_STATE_PAUSE;
	pMachine->state = PX_MACHINE_STATE_PAUSE;

	if (!PX_AbiSet_dword(preturn_abi, "ip", pthread->ip))
		return PX_FALSE;

	if (!PX_AbiSet_string(preturn_abi, "return", "ok"))
		return PX_FALSE;
	return PX_TRUE;
}


px_bool PX_Machine_Execute(PX_Machine* pMachine, const px_byte abi_payload[], px_int payload_size, px_abi* abi_writer_return)
{
	const px_char *popcode;
	px_dword *pid;
	px_abi payload_abi;
	PX_AbiCreate_StaticReader(&payload_abi, abi_payload, payload_size);
	if(!PX_AbiCheck(&payload_abi))
	{
		//drop malformed packet
		return PX_FALSE;
	}
    popcode=PX_AbiGet_string(&payload_abi, "opcode");
	if(!popcode)
	{
		//drop packet without opcode
		return PX_FALSE;
	}
	pid = PX_AbiGet_dword(&payload_abi, "id");
	if (!pid)
	{
		//drop packet without id
		return PX_FALSE;
	}

	if (*pid==pMachine->last_request_id)
	{
		return PX_FALSE;
	}

	pMachine->last_request_id = *pid;

	//mirror opcode and id back to the response so the client can dispatch
	if (!PX_AbiSet_string(abi_writer_return, "opcode", popcode))
		return PX_FALSE;
	if (!PX_AbiSet_dword(abi_writer_return, "id", *pid))
		return PX_FALSE;

	if (PX_strequ(popcode, "get_state"))
		return PX_Machine_Execute_Opcode_get_state(pMachine, &payload_abi, abi_writer_return);
	else if (PX_strequ(popcode, "load"))
		return PX_Machine_Execute_Opcode_load(pMachine, &payload_abi, abi_writer_return);
	else if (PX_strequ(popcode, "run"))
		return PX_Machine_Execute_Opcode_run(pMachine, &payload_abi, abi_writer_return);
	else if (PX_strequ(popcode, "pause"))
		return PX_Machine_Execute_Opcode_pause(pMachine, &payload_abi, abi_writer_return);
	else if (PX_strequ(popcode, "step"))
		return PX_Machine_Execute_Opcode_step(pMachine, &payload_abi, abi_writer_return);
	else if (PX_strequ(popcode, "reset"))
		return PX_Machine_Execute_Opcode_reset(pMachine, &payload_abi, abi_writer_return);
	else if (PX_strequ(popcode, "stop"))
		return PX_Machine_Execute_Opcode_stop(pMachine, &payload_abi, abi_writer_return);
	else if (PX_strequ(popcode, "set_register"))
		return PX_Machine_Execute_Opcode_set_register(pMachine, &payload_abi, abi_writer_return);
	else if (PX_strequ(popcode, "set_memorysize"))
		return PX_Machine_Execute_Opcode_set_memorysize(pMachine, &payload_abi, abi_writer_return);
	else if (PX_strequ(popcode, "set_stacksize"))
		return PX_Machine_Execute_Opcode_set_stacksize(pMachine, &payload_abi, abi_writer_return);
	else if (PX_strequ(popcode, "read_memory"))
		return PX_Machine_Execute_Opcode_read_memory(pMachine, &payload_abi, abi_writer_return);
	else if (PX_strequ(popcode, "write_memory"))
		return PX_Machine_Execute_Opcode_write_memory(pMachine, &payload_abi, abi_writer_return);
	else if (PX_strequ(popcode, "breakpoint"))
		return PX_Machine_Execute_Opcode_breakpoint(pMachine, &payload_abi, abi_writer_return);
	else if (PX_strequ(popcode, "memory_breakpoint"))
		return PX_Machine_Execute_Opcode_memory_breakpoint(pMachine, &payload_abi, abi_writer_return);
	else if (PX_strequ(popcode, "call"))
		return PX_Machine_Execute_Opcode_ucall(pMachine, &payload_abi, abi_writer_return);

	if (!PX_AbiSet_string(abi_writer_return, "return", "unknown opcode"))
		return PX_FALSE;
	return PX_TRUE;
}
