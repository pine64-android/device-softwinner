#ifndef __HAL_CAMERA_PLATFORM_H__
#define __HAL_CAMERA_PLATFORM_H_



#ifdef __A80__
#define __PLATFORM_A80__
#define BUFFER_PHY_OFFSET 0
#define GPU_BUFFER_ALIGN

//#define __OPEN_FACEDECTION__
//#define __OPEN_SMILEDECTION__
//#define __OPEN_BLINKDECTION__
//#define __OPEN_APPERCEIVEPEOPLE__

#define __CEDARX_FRAMEWORK_2__

#define USE_ION_MEM_ALLOCATOR
#endif

#ifdef __A83__
#define __PLATFORM_A83__
#define BUFFER_PHY_OFFSET 0
#define GPU_BUFFER_ALIGN ALIGN_4K

#define __CEDARX_FRAMEWORK_2__

#define __OPEN_FACEDECTION__
#define __OPEN_SMILEDECTION__
#define __OPEN_SMARTDECTION__
//#define __OPEN_BLINKDECTION__


#define USE_ION_MEM_ALLOCATOR
#define WATI_FACEDETECT
#endif

#ifdef __A33__
#define __PLATFORM_A33__
#define BUFFER_PHY_OFFSET 0
#define GPU_BUFFER_ALIGN

#define __CEDARX_FRAMEWORK_2__

//#define __OPEN_FACEDECTION__
//#define __OPEN_SMILEDECTION__
//#define __OPEN_SMARTDECTION__
//#define __OPEN_BLINKDECTION__


#define USE_ION_MEM_ALLOCATOR
//#define WATI_FACEDETECT
#endif

#ifdef __A64__
#define __PLATFORM_A64__
#define BUFFER_PHY_OFFSET 0
#define GPU_BUFFER_ALIGN

#define __CEDARX_FRAMEWORK_2__

//#define __OPEN_FACEDECTION__
//#define __OPEN_SMILEDECTION__
//#define __OPEN_SMARTDECTION__
//#define __OPEN_BLINKDECTION__


#define USE_ION_MEM_ALLOCATOR
//#define WATI_FACEDETECT
#endif


#ifdef USE_ION_MEM_ALLOCATOR
extern "C" int ion_alloc_open();
extern "C" int ion_alloc_close();
extern "C" int ion_alloc_alloc(int size);
extern "C" void ion_alloc_free(void * pbuf);
extern "C" int ion_alloc_vir2phy(void * pbuf);
extern "C" int ion_alloc_phy2vir(void * pbuf);
extern "C" void ion_flush_cache(void* startAddr, int size);
extern "C" void ion_flush_cache_all();

#define camera_phy_alloc_open()      ion_alloc_open() 
#define camera_phy_alloc_close()     ion_alloc_close()
#define camera_phy_alloc_alloc(x)    ion_alloc_alloc(x)
#define camera_phy_alloc_free(x)     ion_alloc_free(x)
#define camera_phy_alloc_vir2phy(x)  ion_alloc_vir2phy(x)
#define camera_phy_alloc_phy2vir(x)  ion_alloc_phy2vir(x)
#define camera_phy_flush_cache(x,y)  ion_flush_cache(x,y)
#define camera_phy_flush_cache_all() ion_flush_cache_all
 
#elif USE_SUNXI_MEM_ALLOCATOR
extern "C" int sunxi_alloc_open();
extern "C" int sunxi_alloc_close();
extern "C" int sunxi_alloc_alloc(int size);
extern "C" void sunxi_alloc_free(void * pbuf);
extern "C" int sunxi_alloc_vir2phy(void * pbuf);
extern "C" int sunxi_alloc_phy2vir(void * pbuf);
extern "C" void sunxi_flush_cache(void * startAddr, int size);
extern "C" void sunxi_flush_cache_all();

#define camera_phy_alloc_open        sunxi_alloc_open() 
#define camera_phy_alloc_close       sunxi_alloc_close()
#define camera_phy_alloc_alloc(x)    sunxi_alloc_alloc(x)
#define camera_phy_alloc_free(x)     sunxi_alloc_free(x)
#define camera_phy_alloc_vir2phy(x)  sunxi_alloc_vir2phy(x)
#define camera_phy_alloc_phy2vir(x)  int sunxi_alloc_phy2vir(x);
#define camera_phy_flush_cache(x,y)  sunxi_flush_cache(x,y);
#define camera_phy_flush_cache_all() sunxi_flush_cache_all()
#endif

#endif
