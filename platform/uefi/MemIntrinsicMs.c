
typedef UINTN size_t;

int
memcmp (
  void *,
  void *,
  size_t
  );

int
memcmp (
  const void  *s1,
  const void  *s2,
  size_t      n
  )
{
  unsigned char const  *t1;
  unsigned char const  *t2;

  t1 = s1;
  t2 = s2;

  while (n-- != 0) {
    if (*t1 != *t2) {
      return (int)*t1 - (int)*t2;
    }

    t1++;
    t2++;
  }

  return 0;
}

void *
memcpy (
  void *,
  const void *,
  size_t
  );

void *
memcpy (
  void        *dest,
  const void  *src,
  size_t      n
  )
{
  unsigned char        *d;
  unsigned char const  *s;

  d = dest;
  s = src;

  while (n-- != 0) {
    *d++ = *s++;
  }

  return dest;
}

void *
memmove (
  void *,
  const void *,
  size_t
  );

void *
memmove (
  void        *dest,
  const void  *src,
  size_t      n
  )
{
  unsigned char        *d;
  unsigned char const  *s;

  d = dest;
  s = src;

  if (d < s) {
    while (n-- != 0) {
      *d++ = *s++;
    }
  } else {
    d += n;
    s += n;
    while (n-- != 0) {
      *--d = *--s;
    }
  }

  return dest;
}

void *
memset (
  void *,
  int,
  size_t
  );

void *
memset (
  void    *s,
  int     c,
  size_t  n
  )
{
  unsigned char  *d;

  d = s;

  while (n-- != 0) {
    *d++ = (unsigned char)c;
  }

  return s;
}
