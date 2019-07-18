<img src="https://raw.githubusercontent.com/matrixcascade/PainterEngine/master/logo/foxlogo.png">

# PainterEngine API manual

<p style="float:right">Author:DBinary</p>
<p style="float:right">Matrixcascade@gmail.com</p>

[TOC]

## Core

### 预编译

#### 计算数组个数

```c
#define px_countof(x) (sizeof(x) / sizeof(x[0]))
```

### 内存

#### 内存操作

##### 拷贝内存

```c
px_void px_memcpy(px_void* dst, px_void* src, px_uint size);
```

* 参数

  - dst

    目标内存指针

  - src

    源内存指针
    
  - count

    拷贝内存大小

##### 设置内存值

```c
px_void px_memset(px_void* dst, px_byte byte, px_uint size);
```

* 参数

  - dst

    目标内存指针

  - byte

    值

  - size

    设置内存大小返回值

##### 按四字节对齐设置内存值

```c
px_void px_memdwordset(px_void* dst, px_dword dw, px_uint count);
```

* 参数

  - dst

    目标内存指针

  - dw

    值

  - size

    需要设置的四字节个数

##### 比较目标内存是否相同

```c
px_bool px_memequ(px_void* dst, px_void* src, px_uint size);
```

* 参数

  - dst

    目标内存指针

  - src

    源内存指针

  - size

    比较内存大小

* 返回值

  - 相同为PX_TRUE，否则为PX_FALSE

#### 内存池

##### 创建内存池

```c
px_memorypool MP_Create(px_void* addr, px_uint size);
```

* 参数

  - addr

    内存池的开始地址

  - size

    需要创建的内存池的大小

* 返回值

  - 内存池结构

##### 返回一块内存指针所指向内存的内存大小

```c
px_uint MP_Size(px_memorypool* pool, px_void* ptr);
```

* 参数

  - pool

    内存池指针

  - ptr

    指针

* 返回值

  - 该指针所指向内存块大小


##### 从内存池分配一块内存

```c
px_void* MP_Malloc(px_memorypool* pool, px_uint* size);
```

* 参数

  - pool

    内存池指针

  - size

    需要分配的大小

* 返回值

  - 成功为内存指针，失败为PX_NULL

* 备注

  - 该内存将会优先从已回收内存节点中分配，如果回收节点无法分配再从空余空间分配

##### 从内存池释放一块内存

```c
px_void MP_Free(px_memorypool* pool, px_uint* ptr);
```

* 参数

  - pool

    内存池指针

  - ptr

    清空的内存首地址指针

##### 重置内存池

```c
px_void MP_Reset(px_memorypool* pool);
```

* 参数

  - pool

    内存池指针

* 备注

  - 将清空内存池中所有节点

##### 释放内存池

```c
px_void MP_Release(px_memorypool* pool);
```

* 参数

  - pool

    内存池指针

* 备注

  - 将销毁清空内存池

##### 设置内存池异常回调

```c
typedef enum {
	PX_MEMORYPOOL_ERROR_OUTOFMEMORY,
	PX_MEMORYPOOL_ERROR_INVALID_ACCESS,
	PX_MEMORYPOOL_ERROR_INVALID_ADDRESS
} PX_MEMORYPOOL_ERROR;
typedef px_void (*PX_MP_ErrorCall)(PX_MEMORYPOOL_ERROR)
px_void MP_ErrorCatch(px_memorypool* pool, PX_MP_ErrorCall errorcall);
```

* 参数

  - pool

    内存池指针

  - errorcall

    回调函数

* 备注

  - 若未设置，会调用PX_ERROR

  - 异常

    - PX_MEMORYPOOL_ERROR_OUTOFMEMORY

      内存池内存不足
    
    - PX_MEMORYPOOL_ERROR_INVALID_ACCESS
      
      内存池指向内存无法读/写/访问
      
    - PX_MEMORYPOOL_ERROR_INVALID_ADDRESS

      释放无效的内存地址

#### 字符串操作

##### 字符串拷贝

```c
px_void px_strcpy(px_char* dst, px_char* src, px_uint size);
```

* 参数

  - dst

    目标字符串指针

  - src

    源字符串指针

  - size

    字符串长度

##### 字符串长度

```c
px_int px_strlen(px_char* src);
```

* 参数

  - src

    源字符串指针
  
* 返回值

  - 字符串长度
