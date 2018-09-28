


const AVOption *av_opt_next(const void *obj, const AVOption *last)
{
    const AVClass *class_para;
    if (!obj)
        return NULL;
    
    class_para = *(const AVClass**)obj;
    if (!last && class_para && class_para->option && class_para->option[0].name)
        return class_para->option;
    
    if (last && last[1].name)
        return ++last;
    
    return NULL;
}

void av_opt_free(void *obj)
{
    const AVOption *o = NULL;
    while ((o = av_opt_next(obj, o))) {
        switch (o->type) {
        case AV_OPT_TYPE_STRING:
        case AV_OPT_TYPE_BINARY:
            ff_freep((uint8_t *)obj + o->offset);
            break;

        case AV_OPT_TYPE_DICT:
            av_dict_free((AVDictionary **)(((uint8_t *)obj) + o->offset));
            break;

        default:
            break;
        }
    }
}

static int write_number(void *obj, const AVOption *o, void *dst, double num, int den, int64_t intnum)
{
    if (o->type != AV_OPT_TYPE_FLAGS &&
        (!den || o->max * den < num * intnum || o->min * den > num * intnum)) {
        num = den ? num * intnum / den : (num * intnum ? INFINITY : NAN);
        //av_log(obj, AV_LOG_ERROR, "Value %f for parameter '%s' out of range [%g - %g]\n", num, o->name, o->min, o->max);
        return -1;//AVERROR(ERANGE);
    }
    if (o->type == AV_OPT_TYPE_FLAGS) {
        double d = num*intnum/den;
        if (d < -1.5 || d > 0xFFFFFFFF+0.5 || (llrint(d*256) & 255)) {
            /*av_log(obj, AV_LOG_ERROR, "Value %f for parameter '%s' is not a valid set of 32bit integer flags\n",
                   num*intnum/den, o->name);*/
            return -1;//AVERROR(ERANGE);
        }
    }

    switch (o->type) {
    case AV_OPT_TYPE_PIXEL_FMT:
        //*(enum AVPixelFormat *)dst = llrint(num / den) * intnum;
        break;
    case AV_OPT_TYPE_SAMPLE_FMT:
        //*(enum AVSampleFormat *)dst = llrint(num / den) * intnum;
        break;
    case AV_OPT_TYPE_BOOL:
    case AV_OPT_TYPE_FLAGS:
    case AV_OPT_TYPE_INT:
        *(int *)dst = llrint(num / den) * intnum;
        break;
    case AV_OPT_TYPE_DURATION:
    case AV_OPT_TYPE_CHANNEL_LAYOUT:
    case AV_OPT_TYPE_INT64:
        double d = num / den;
        if (intnum == 1 && d == (double)INT64_MAX) {
            *(int64_t *)dst = INT64_MAX;
        } else
            *(int64_t *)dst = llrint(d) * intnum;
        break;
    case AV_OPT_TYPE_UINT64:
        double d = num / den;
        // We must special case uint64_t here as llrint() does not support values
        // outside the int64_t range and there is no portable function which does
        // "INT64_MAX + 1ULL" is used as it is representable exactly as IEEE double
        // while INT64_MAX is not
        if (intnum == 1 && d == (double)UINT64_MAX) {
            *(uint64_t *)dst = UINT64_MAX;
        } else if (d > INT64_MAX + 1ULL) {
            *(uint64_t *)dst = (llrint(d - (INT64_MAX + 1ULL)) + (INT64_MAX + 1ULL))*intnum;
        } else {
            *(uint64_t *)dst = llrint(d) * intnum;
        }
        break;
    case AV_OPT_TYPE_FLOAT:
        *(float *)dst = num * intnum / den;
        break;
    case AV_OPT_TYPE_DOUBLE:
        *(double *)dst = num * intnum / den;
        break;
    case AV_OPT_TYPE_RATIONAL:
    case AV_OPT_TYPE_VIDEO_RATE:
        if ((int) num == num)
            *(AVRational *)dst = (AVRational) { num *intnum, den };
        else
            //*(AVRational *)dst = av_d2q(num * intnum / den, 1 << 24);
        break;
    default:
        return -1;//AVERROR(EINVAL);
    }
    
    return 0;
}

static int set_string_color(void *obj, const AVOption *o, const char *val, uint8_t *dst)
{
    int ret = 0;

    if (!val) {
        return 0;
    } else {
        //ret = av_parse_color(dst, val, -1, obj);
        if (ret < 0)
            //av_log(obj, AV_LOG_ERROR, "Unable to parse option value \"%s\" as color\n", val);
        return ret;
    }
    return 0;
}

static int set_string(void *obj, const AVOption *o, const char *val, uint8_t **dst)
{
    av_freep(dst);
    *dst = av_strdup(val);
    return *dst ? 0 : -1;//AVERROR(ENOMEM);
}

static int set_string_image_size(void *obj, const AVOption *o, const char *val, int *dst)
{
    int ret = 0;

    if (!val || !strcmp(val, "none")) {
        dst[0] =
        dst[1] = 0;
        return 0;
    }

    /*
    ret = av_parse_video_size(dst, dst + 1, val);
    if (ret < 0)
        av_log(obj, AV_LOG_ERROR, "Unable to parse option value \"%s\" as image size\n", val);
    */
    
    return ret;
}

static int set_string_video_rate(void *obj, const AVOption *o, const char *val, AVRational *dst)
{
    int ret = 0;
    if (!val) {
        ret = -1;//AVERROR(EINVAL);
    } else {
        //ret = av_parse_video_rate(dst, val);
    }
    
    if (ret < 0)
        //av_log(obj, AV_LOG_ERROR, "Unable to parse option value \"%s\" as video rate\n", val);

    return ret;
}

static int hexchar2int(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

static int set_string_binary(void *obj, const AVOption *o, const char *val, uint8_t **dst)
{
    int *lendst = (int *)(dst + 1);
    uint8_t *bin, *ptr;
    int len;

    av_freep(dst);
    *lendst = 0;

    if (!val || !(len = strlen(val)))
        return 0;

    if (len & 1)
        return -1;//AVERROR(EINVAL);
    len /= 2;

    ptr = bin = av_malloc(len);
    if (!ptr)
        return -1;//AVERROR(ENOMEM);
        
    while (*val) {
        int a = hexchar2int(*val++);
        int b = hexchar2int(*val++);
        if (a < 0 || b < 0) {
            av_free(bin);
            return -1;//AVERROR(EINVAL);
        }
        *ptr++ = (a << 4) | b;
    }
    *dst    = bin;
    *lendst = len;

    return 0;
}


void av_opt_set_defaults2(void *s, int mask, int flags)
{
    const AVOption *opt = NULL;
    while ((opt = av_opt_next(s, opt))) {
        void *dst = ((uint8_t*)s) + opt->offset;

        if ((opt->flags & mask) != flags)
            continue;

        if (opt->flags & AV_OPT_FLAG_READONLY)
            continue;

        switch (opt->type) {
            case AV_OPT_TYPE_CONST:
                /* Nothing to be done here */
                break;
            case AV_OPT_TYPE_BOOL:
            case AV_OPT_TYPE_FLAGS:
            case AV_OPT_TYPE_INT:
            case AV_OPT_TYPE_INT64:
            case AV_OPT_TYPE_UINT64:
            case AV_OPT_TYPE_DURATION:
            case AV_OPT_TYPE_CHANNEL_LAYOUT:
            case AV_OPT_TYPE_PIXEL_FMT:
            case AV_OPT_TYPE_SAMPLE_FMT:
                write_number(s, opt, dst, 1, 1, opt->default_val.i64);
                break;
            case AV_OPT_TYPE_DOUBLE:
            case AV_OPT_TYPE_FLOAT: {
                double val;
                val = opt->default_val.dbl;
                write_number(s, opt, dst, val, 1, 1);
            }
            break;
            case AV_OPT_TYPE_RATIONAL: {
                AVRational val;
                //val = av_d2q(opt->default_val.dbl, INT_MAX);
                write_number(s, opt, dst, 1, val.den, val.num);
            }
            break;
            case AV_OPT_TYPE_COLOR:
                set_string_color(s, opt, opt->default_val.str, dst);
                break;
            case AV_OPT_TYPE_STRING:
                set_string(s, opt, opt->default_val.str, dst);
                break;
            case AV_OPT_TYPE_IMAGE_SIZE:
                set_string_image_size(s, opt, opt->default_val.str, dst);
                break;
            case AV_OPT_TYPE_VIDEO_RATE:
                set_string_video_rate(s, opt, opt->default_val.str, dst);
                break;
            case AV_OPT_TYPE_BINARY:
                set_string_binary(s, opt, opt->default_val.str, dst);
                break;
            case AV_OPT_TYPE_DICT:
                /* Cannot set defaults for these types */
            break;
        default:
            //av_log(s, AV_LOG_DEBUG, "AVOption type %d of option %s not implemented yet\n", opt->type, opt->name);
        }
    }
}

void av_opt_set_defaults(void *s)
{
    av_opt_set_defaults2(s, 0, 0);
}