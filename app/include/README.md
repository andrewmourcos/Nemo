# Nemo/includes
All header files will live here. On all files, please put include-guards. Here's an example if your new file is named `grandparent.h`:
```cpp
#ifndef __GRANDPARENT_H__
#define __GRANDPARENT_H__

// --- this is just random code
struct foo { 
    int member;
};
// --- this is just random code

#endif /* __GRANDPARENT_H__ */

```