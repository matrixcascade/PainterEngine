/*          PainterEngine Compiler Architecture
*
* /////////////////////////////////////////////////////////////
*			 ------------------------------>
*			^								|
*			|								* 
*    syntax_source--->syntax_optimizer-->syntax_ir--->machine
* 
* //////////////////////////////////////////////////////////////
*

             PainterEngine MicroSoc(CPU FPU GPU) Architecture
*    +--------------+           +------------------------+
*    |     ALU      |           |           FPU          |
*    +--------------+           | f0.f1.f2.f3.C0.C2.C3   |
*           |                   +------------------------+
*           |                          |
*   +-------------------------------------------+    
*   |               CPU Registers               |----------------+
*   |       r0...r3 | ip | sp | bp | flag(ZCNV) |                |
*   +-------------------------------------------+                |
*                        |                                       |
*  +--------------------------------------------+    +-------------------------+
*  |                 Memory                     |----|         GPU             |
*  +--------------------------------------------+    +-------------------------+
* 
* r0...r3 integer
* f0...f3 float
* r4...r7 ip,sp,bp,flag
* 64bits for instruction

//pseudo-instruction
import module_name(string)
label_name:
export label_name:
db label_name:  string(eg:ff00ff001020....)

//opcode 1byte  1bit for extern 7bit for opcode
//nop
nop

* register / const to register
movr[r0...r7,f0...f3], [r0...r7,f0...f3] //3bytes 8bits for opcode.(0~3bit)4bit for dest type(0 common,1 float),(3~7bit)4bit for index,(0~3bit)4bit for source type(0 common,1 float),(3~7bit)4bit for index
movc[r0...r7], [const] //6-bytes 8bits for opcode.(0~3bit)3bit for register index,4bits reserved, 32bits for const
movf[r0...r7,f0..f3], [const] //6-bytes 8bits for opcode.(0~2bit)3bit for register index,1bits flag(0 common 1 float) .32bits for const float

* memory to memory
movn ;r0(source address), r1(destination address), r2(size) //1-byte opcode

f2i [r0...r1][f0...f1]  fx->rx //16bits // 8bits for opcode.(0~3)4bit for common register index,(4~7)4bit for float register index
i2f [f0...f1][r0...r1]  rx->fx //16bits // 8bits for opcode.(0~3)4bit for float register index,(4~7)4bit for common register index
u2f [f0...f1][r0...r1]  rx->fx //16bits // 8bits for opcode.(0~3)4bit for float register index,(4~7)4bit for common register index
ff2f //8 bit for opcode,copy fflag to flag register
* memory to register
* 
loadu8[r0...r1], [n][bp - n] //6-bytes 8bits for opcode.4bit for register,4bit for local(1)/global(0) 32bits for address offset
loadu16[r0...r1], [n][bp - n] //6-bytes 8bits for opcode.4bit for register,4bit for local(1)/global(0) 32bits for address offset
loadu32[r0...r1], [n][bp - n] //6-bytes 8bits for opcode.4bit for register,4bit for local(1)/global(0) 32bits for address offset

loadi8[r0...r1], [n][bp - n] //6-bytes 8bits for opcode.4bit for register,4bit for local(1)/global(0) 32bits for address offset
loadi16[r0...r1], [n][bp - n] //6-bytes 8bits for opcode.4bit for register,4bit for local(1)/global(0) 32bits for address offset
loadi32[r0...r1], [n][bp - n] //6-bytes 8bits for opcode.4bit for register,4bit for local(1)/global(0) 32bits for address offset


* register / const to memory
store8 [n][bp - n], [r0...r3] //6-bytes 8bits for opcode.4bit for register,4bit for local/global 32bits for address offset
store16 [n][bp - n], [r0...r3] //6-bytes 8bits for opcode.4bit for register,4bit for local/global 32bits for address offset
store32 [n][bp - n], [r0...r3] //6-bytes 8bits for opcode.4bit for register,4bit for local/global 32bits for address offset


* stack operations
push [r0...r3, ip, sp, bp, flag] //2-bytes 8bits for opcode.(4~7)4bit for register index(4-ip 5-sp 6-bp 7-flag),(0~3)4bit for type(0-reserved 1-register)
pop[r0...r3, ip, sp, bp, flag]  //2-bytes 8bits for opcode.4bit for register index(4-ip 5-sp 6-bp 7-flag),4bit for type(0~3)(0-none 1-register)
popn [r0] //1-bytes 8bits for opcode
addsp [n] //5-bytes 8bits for opcode.32bits for n
subsp [n] //5-bytes 8bits for opcode.32bits for n

*ALU operations
* Arithmetic
* neg [r0...r7] // 2bytes 8bits for opcode.4bit for register index
* add //rx+rx->rx //3bytes 8bits for opcode,4bit for dest index,4bit for src1 index,4bit for src2 index(0-3 4-7 0-3 4b zero append)
* sub //rx-rx->rx
* mul //rx*rx->rx
* div //rx/rx->rx
* idiv //rx/rx->rx (signed)
* mod //rx%rx->rx (signed)
* 
* FPU operations
* fneg [f0...f3]
* fadd //fx+fx->fx
* fsub //fx-fx->fx
* fmul //fx*fx->fx
* fdiv //fx/fx->fx
* fcmp //fx-fx
* fcomi //fx-fx direct compare and set flag
* ff2f // fflag->flag

* Logical
* and //3bytes 8bits for opcode,4bit for dest index,4bit for src1 index,4bit for src2 index(0-3 4-7 0-3 4b zero append)
* or  //3bytes 8bits for opcode,4bit for dest index,4bit for src1 index,4bit for src2 index(0-3 4-7 0-3 4b zero append)
* xor //3bytes 8bits for opcode,4bit for dest index,4bit for src1 index,4bit for src2 index(0-3 4-7 0-3 4b zero append)
* not [r0...r3] //2bytes 8bits for opcode.4bit for register index
* shift left //3bytes 8bits for opcode,4bit for dest index,4bit for src1 index,4bit for src2 index(0-3 4-7 0-3 4b zero append)
* shift right //3bytes 8bits for opcode,4bit for dest index,4bit for src1 index,4bit for src2 index(0-3 4-7 0-3 4b zero append)
* Comparison
* cmp r0,r1 //2 bytes 8bits for opcode.4bit for r0 index,4bit for r1 index
* jmp [label] //5 bytes 8bits for opcode.32bits for label address
* je  [label] //r0==r1?
* jne [label] //r0!=r1?
* jg  [label] //r0>r1?
* jl  [label] //r0<r1?
* jge [label] //r0>=r1?
* jle [label] //r0<=r1?
* call [label] // 8bits for opcode. 32bits for label address
* 
* jmpr [r0] // 8bits for opcode.8bit for regiser-index
* callr [r0] // 8bits for opcode.8bit for regiser-index
* 
* 
* end-to-end instructions
* 
* 
* ee_push [const,rx,fx,addr,bp-addr] //6-bytes 8bits for opcode.8bit for type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr),32bits for address
* ee_pop [const,rx,fx,addr,bp-addr] //6-bytes 8bits for opcode.8bit for type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr),32bits for address

* ee_neg [rx,fx,addr,bp-addr] // 6bytes 8bits for opcode.8bit for type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr),32bits for address
* ee_add [rx,fx,addr,bp-addr],[const_int,rx,addr,bp-addr],[const_int,rx,addr,bp-addr] //16 bytes 8bits for opcode,8bit for dest type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr)x3,32bits for dest address,op1,op2 address
* ee_sub [rx,fx,addr,bp-addr],[const_int,rx,addr,bp-addr],[const_int,rx,addr,bp-addr] //16 bytes 8bits for opcode,8bit for dest type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr)x3,32bits for dest address,op1,op2 address
* ee_mul [rx,fx,addr,bp-addr],[const_int,rx,addr,bp-addr],[const_int,rx,addr,bp-addr] //16 bytes 8bits for opcode,8bit for dest type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr)x3,32bits for dest address,op1,op2 address
* ee_div [rx,fx,addr,bp-addr],[const_int,rx,addr,bp-addr],[const_int,rx,addr,bp-addr] //16 bytes 8bits for opcode,8bit for dest type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr)x3,32bits for dest address,op1,op2 address
* ee_idiv [rx,fx,addr,bp-addr],[const_int,rx,addr,bp-addr],[const_int,rx,addr,bp-addr] //16 bytes 8bits for opcode,8bit for dest type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr)x3,32bits for dest address,op1,op2 address
* ee_mod [rx,fx,addr,bp-addr],[const_int,rx,addr,bp-addr],[const_int,rx,addr,bp-addr] //16 bytes 8bits for opcode,8bit for dest type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr)x3,32bits for dest address,op1,op2 address
*
* FPU operations
* ee_fneg [rx,fx,addr,bp-addr] // 6bytes 8bits for opcode.8bit for type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr),32bits for address
* ee_fadd [rx,fx,addr,bp-addr],[const,fx,addr,bp-addr],[const,fx,addr,bp-addr] //16 bytes 8bits for opcode,8bit for dest type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr)x3,32bits for dest address,op1,op2 address
* ee_fsub [rx,fx,addr,bp-addr],[const,fx,addr,bp-addr],[const,fx,addr,bp-addr] //16 bytes 8bits for opcode,8bit for dest type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr)x3,32bits for dest address,op1,op2 address
* ee_fmul [rx,fx,addr,bp-addr],[const,fx,addr,bp-addr],[const,fx,addr,bp-addr] //16 bytes 8bits for opcode,8bit for dest type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr)x3,32bits for dest address,op1,op2 address
* ee_fdiv [rx,fx,addr,bp-addr],[const,fx,addr,bp-addr],[const,fx,addr,bp-addr] //16 bytes 8bits for opcode,8bit for dest type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr)x3,32bits for dest address,op1,op2 address
*

* Logical
* ee_and [rx,fx,addr,bp-addr],[const_int,rx,addr,bp-addr],[const_int,rx,addr,bp-addr] //16 bytes 8bits for opcode,8bit for dest type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr)x3,32bits for dest address,op1,op2 address
* ee_or  [rx,fx,addr,bp-addr],[const_int,rx,addr,bp-addr],[const_int,rx,addr,bp-addr] //16 bytes 8bits for opcode,8bit for dest type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr)x3,32bits for dest address,op1,op2 address
* ee_xor [rx,fx,addr,bp-addr],[const_int,rx,addr,bp-addr],[const_int,rx,addr,bp-addr] //16 bytes 8bits for opcode,8bit for dest type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr)x3,32bits for dest address,op1,op2 address
* ee_not [rx,fx,addr,bp-addr] // 6bytes 8bits for opcode.8bit for type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr),32bits for address
* ee_shift left [rx,fx,addr,bp-addr],[const_int,rx,fx,addr,bp-addr],[const_int,rx,fx,addr,bp-addr] //16 bytes 8bits for opcode,8bit for dest type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr)x3,32bits for dest address,op1,op2 address
* ee_shift right [rx,fx,addr,bp-addr],[const_int,rx,fx,addr,bp-addr],[const_int,rx,fx,addr,bp-addr] //16 bytes 8bits for opcode,8bit for dest type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr)x3,32bits for dest address,op1,op2 address

* Comparison
* ee_cmp [const_int,rx,fx,addr,bp-addr],[const_int,rx,fx,addr,bp-addr] //12 bytes 8bits for opcode,8bit for op type*2(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr),8bit zero append,32bits for op1,32bits for op2
* ee_fcmp [const_float,rx,fx,addr,bp-addr],[const_float,rx,fx,addr,bp-addr] //12 bytes 8bits for opcode,8bit for op type*2(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr),8bit zero append,32bits for op1,32bits for op2
* ee_call [const_int,rx,fx,addr,bp-addr,label] //6-bytes 8bits for opcode.8bit for type(0-const_int 1-const_float 2-rx 3-fx 4-addr 5-bp-addr),32bits for address

/*symbols
@;go_source source_index(const_int)
@;new begin(const_int) end(const_int)
@;color argb(const_int_list)
@;info info(string)
@;local source_index(const_int) variable_index(const_int)

*/
// is_eof(function)

// define -- define_identifier --- value(string)
//       |
//        -- define_parameters --- identifier[] --- identifier[] --- value(string)
//       |                    |
//       |                     --- count --- value(string)
//		 |
//        -- define_content --- value(string)
//       |
//        -- format(string)

// call_define_identifier --- value(string)
//                       |
//                        --- macro_id(string)

// call_define(function)

//expand_parameters --- expand_parameter[] --- value(string)
//				   |
//					--- count(int)

// keyword(function) -- *[]["if","else","switch","case","default",\
		"while","for","do","break","continue","return","goto","typedef",\
		"extern","static","auto","register","const","volatile","sizeof",\
		"enum","struct","union","void","char","short","int","long","float",\
		"double","signed","unsigned","_Bool","_Complex","_Imaginary","inline",\
		"restrict","_Alignas","_Alignof","_Atomic","_Generic","_Noreturn",\
		"_Static_assert","_Thread_local"];

// identifier --- value(string)
//           |
//            --- source_index(int)
//           |
//            --- begin(int)
//           |
//            --- end(int)



// const_float --- value(string)


// const_int --- value(string)
//          |
//           --- unsigned(bool)

// numeric --- const_int(abi)
//        |
//         --- const_float(abi)

// bcontainer --- value(string)

// const_string --- value(sting)


// const_float_list  --- const_float[](abi)
//                 |
//                  --- list_count(int)

// const_int_list    --- const_int[](abi)
//                  |
//                   --- list_count(int)

// const_numeric_list --- const_numeric[](abi)
//                   |
//                    --- list_count(int)

// const_string_list  --- const_string[](abi)
//                   |
//                    --- list_count(int)

// const_tuple_list   --- const_tuple[](abi)
//                   |
//                    --- list_count(int)

//  numeric --- const_int(abi)
//         |
//          --- const_float(abi)
//         |
//          --- const_string(abi)

// declare_prefix --- value ['const','static','unsigned'](string)

// declare_prefixs --- [](string)

// declare_token_prefix --- type ['pointer','reference'](string)
//                     |
//                      --- pointer_level(int)

// declare_array   --- d(int)
//                |
//                 --- [d](int 0~d-1)
//				  |
//				   --- count(int)

// const_int_set --- const_int_list(abi)
// const_float_set --- const_float_list(abi)
// const_string_set --- const_string_list(abi)
// const_tuple_set --- const_tuple_list(abi)

// define_struct  --- struct_name --- value(string)
//				 |
//				 |--- member_count(int)
//				 |
//				 |--- size(int)
//				 |
//				 |--- [](variable abi)


//identifier
//pointer:any
//ix.'u8/16/32/i8/16/32/const',
//fx.'f.32/const'
//void
//struct.'struct_name'
// 
// scope  --- alloc(int)
//		 |
//        -- lifetime(int 1:global others:local or define)
//       |
//        --- type_mnemonic---string->string(eg :'int'->'ix.i.32','float'->'fx.f.32')
//       |
//        --- type_defines(type_define abis,eg:ix.i.32)      
//       |
//        --- variables--variable_identifier---variable abi

// type  --- value(string) (eg:'ix.i.32','fx.f.32','pointer.array.ix.i.32','struct_name')


// variable --- identifier(string)
//         |
//          --- type(string)
//         |
//          --- type_size(int)
//         |
//          --- size(int) (count = size/type_size)
//         |
//          --- offset(int)
//		   |
//			--- scope(string global/local)
//         |
//          --- lifetime(int)
//		   | 
//			--- *array-- --d(int) [array dimension]
//                      |  
//                       --[](int) [array dimension]
//                      |
//                       --count(int) (count = size/type_size)
//         |
//          --- *pointer_level(int)
//         |
//          --- *decoration(abi)--[](string) (eg:'const','volatile'...)

// 
// type_define(*define_struct)   --- size(int)
//								| 
//								|-- *member_count(int)
//								|
//								|-- members--variable identifier--variable(abi)
//								|
//							     --- operates(operate abis) -- [] -- opcode_index(int) 
//							                                  |
//							                                   -- *other_type(string) (eg:ix:i:8)
//							                                  |
//							                                   -- function(data) PX_Syntax_Operate_Function


// define_function --- function_return_type(abi type)
//				  |
//				   --- function_name(identifier abi)
//				  |
//                 --- param_count(int)
// 				  |
//				   --- [](parameters abi variable)

// call_function   --- value(string)
//                |
//                 --- check_param_count(int)
//				  |
//				   --- define_function(abi define_function)


//pointer:any
//array:any
//ix:u/i/const:8/16/32/i8/16/32',
//fx:'f/const:32'
//void
//struct:'struct_name'

//oprand --- type(string) ['ix.i.32','fx.f.8'...] <----opcode target
//      |
//	     --- type_size(int) 
//      |
//		 --- *array-- --d(int) [array dimension]
//                   |  
//                   --[](int) [array dimension]
//                   |
//                   --count(int) (count = size/type_size)
//		|
//		 --- *pointer_level(int)
//      |
//       --- *value(string) if 'const' then is const value
//		|
//       --- *from(int)
/*
			PX_SYNTAX_OPRAND_FROM_GLOBAL=0,
			PX_SYNTAX_OPRAND_FROM_LOCAL,
			PX_SYNTAX_OPRAND_FROM_REGISTER,
			PX_SYNTAX_OPRAND_FROM_STACK,
			PX_SYNTAX_OPRAND_FROM_CONST,
*/
//      |
//       --- *offset(int)


// opcode --- index(index)


//ir --- value(string)


/*PX_Machine
ir_rx ---- index(int) [r0,r1,r2,r3]

ir_addr --- *global_offset(int)
		 |
		  --- *local_offset(int)


operate  --- oprand_type[](string)
        |
		 --- oprand_count(int)
		|
		--- pfunction(data PX_Syntax_Operate_Function*)


define_opcode --- opcode(string)
			 |
			  --- type(int) [PX_SYNTAX_OPCODE_TYPE]
			 |
			  --- precedence(int)
			 |
			  --- operate_count(int)
			 |
			  --- operate[](abi)

Machine Memory map
=============================================
   ^		^            					^
   |		|     stack(128k default)	 	|
   |		|								|
   |		|================================
   |		|								|
 module x	|         heap(global)			|
memory size	|								|
   |		|================================
   |		|								|
   |		|		rdata segment			|
   |		|								|
   |		|================================
   |		|           					|
   |		|       text segment			|
module_base	|								|
=============================================
Machine Execute abi
*opcode --- ["load","run","pause","reset","stop","get_state","get_registers","set_register","set_memorysize","set_stacksize","read_memory","write_memory","breakpoint","memory_breakpoint","call",]

//id(dword) bin(abi) address/ip(dword)

param bin(abi) --- module_name(string)
              |
               --- export --- name(abi)---address(ip)
			  |
			   --- import --- name(abi)---address(ip)
			  |
			   --- text(data)
			  |
			   --- rdata(data)



opcode load --- bin(abi) --> return_abi: return  -- opcode(string)
		   |							   |
			--- id(dword)					--- "ok"/"error message"
                                           |
                                            ---id(dword)

opcode run ---id(dword) --> return_abi: return  -- opcode(string)
											   |
												--- "ok"/"error message"
											   |
												---id(dword)

opcode pause---id(dword) --> return_abi: return  -- opcode(string)
												|
												 --- "ok"/"error message"
												|
												 ---id(dword)

opcode step  --tick(dword) --> return_abi: return  -- opcode(string)
			|			     					 |
			 --id(dword)						  --- "ok"/"error message"
												 |
												  ---id(dword)
												 |
												  ---ip(dword)

opcode reset---id(dword) --> return_abi: return  -- opcode(string)
												|
												 --- "ok"/"error message"
												|
												 ---id(dword)

opcode stop---id(dword) --> return_abi: return	 -- opcode(string)
												|
												 --- "ok"/"error message"
												|
												 ---id(dword)

opcode get_state---id(dword) --> return_abi:  -- opcode(string)
											|
											  --return "ok"/"error message"
											|
											  ---id(dword)
											|
											  ---state(string) ["idle","running","pause","error"]
											|
											  ---r[0~7](data array of px_dword)
											|
											  ---f[0~3](data array of px_float32)
											|
											  ---ip_breakpoint(data array of px_dword)

opcode set_memorysize---id(dword) --- return_abi: return  --- "ok"/"error message"
													     |
														   ---id(dword)
									   

opcode set_stacksize---id(dword) --> return_abi: return  --- "ok"/"error message"
					|									 |
					 --size(dword)						 ---id(dword)

opcode set_register---register(string) --> return_abi: return  --- "ok"/"error message"
				  |											  |
				   ---value(dword)							   --- id(dword)
				  |
				   ---id(dword)
																				               

opcode read_memory ---address(dword) --> return_abi: return    ---"ok"/"error message"
                   |                                          |
                    ---size(dword)                              --- data(px_byte[])
															  |
															   ---id(dword)

opcode write_memory(id, address, data) --- return_abi: return   ---"ok"/"error message"
															  |
															    ---id(dword)

opcode breakpoint(id, ip) --- return_abi: return   ---"ok"/"error message"
												  |
												   ---id(dword)

opcode memory_breakpoint(id, address, size, mask) --- return_abi: return   ---"ok"/"error message"
															  |
															    ---id(dword)

opcode call(id, address) --- return_abi: return   ---"ok"/"error message"
												 |
												   ---id(dword)
*/