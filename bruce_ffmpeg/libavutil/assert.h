/*********************************
 * Copyright (c) 2018 Bruceshu 3350207067@qq.com
 * Auther:Bruceshu
 * Date:  2018-09-28
 * Description:
 
*********************************/




#if defined(ASSERT_LEVEL) && ASSERT_LEVEL > 1
#define av_assert2(cond) av_assert0(cond)
#define av_assert2_fpu() av_assert0_fpu()
#else
#define av_assert2(cond) ((void)0)
#define av_assert2_fpu() ((void)0)
#endif
