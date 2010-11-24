#include "opl2_int.h"
#include "opl2_vm.h"

/******************************************************************************/

int pl2CharSetName(pl2Character *chr, const char *name)
{
    if(chr)
    {
        if(chr->name)
        {
            free((char*)(chr->name));
            chr->name = NULL;
        }

        if(name) chr->name = strdup(name);

        return name ? (NULL != chr->name) : 1;
    }
    return 0;
}

int pl2CharSetModel(pl2Character *chr, int idx, const char *name)
{
    //DEBUGPRINT("%s: chr == %p, idx == %d, name == \"%s\"\n",
    //           __func__, chr, idx, name);

    if(chr)
    {
        if((idx < 0) || (idx >= PL2_MAX_CHARPARTS))
            return 0;

        if(chr->models[idx])
        {
            pl2ModelFree(chr->models[idx]);
            chr->models[idx] = NULL;
        }

        if(name)
        {
            chr->models[idx] = pl2ModelLoad(name);
        }

        return name ? (NULL != chr->models[idx]) : 1;
    }
    return 0;
}

int pl2CharSetAnim(pl2Character *chr, const char *name)
{
    if(chr)
    {
        if(chr->anim)
        {
            pl2AnimFree(chr->anim);
            chr->anim = NULL;
        }

        if(name)
        {
            chr->anim = pl2AnimLoad(name);
            chr->time = 0;
            chr->frame = -1;
        }

        return name ? (NULL != chr->anim) : 1;
    }
    return 0;
}

void pl2CharAnimate(pl2Character *chr, float dt)
{
    if(chr && chr->anim)
    {
        chr->time += dt;

        if(chr->visible > 0)
        {
            int frame = 30 * chr->time;
            int count = chr->anim->numFrames;
            int loop  = chr->anim->loopFrame;

            if(frame >= count)
            {
                frame = loop + (frame - loop) % (count - loop);
            }

            if(frame != chr->frame)
            {
                int i;
                for(i = 0; i < PL2_MAX_CHARPARTS; i++)
                {
                    pl2ModelAnimate(chr->models[i], chr->anim, frame);
                }

                chr->frame = frame;
            }
        }
    }
}

void pl2CharClear(pl2Character *chr)
{
    if(chr)
    {
        int i;

        for(i = 0; i < PL2_MAX_CHARPARTS; i++)
        {
            if(chr->models[i])
            {
                pl2ModelFree(chr->models[i]);
                chr->models[i] = NULL;
            }
        }

        if(chr->anim)
        {
            pl2AnimFree(chr->anim);
            chr->anim = NULL;
        }

        if(chr->name)
        {
            free((char*)(chr->name));
            chr->name = NULL;
        }

        chr->point = NULL;
        chr->frame = 0;
        chr->time = 0;
        chr->visible = 0;
        chr->black = 0;
        chr->nameColor = 0;
    }
}

