#include "./lv_i18n.h"


////////////////////////////////////////////////////////////////////////////////
// Define plural operands
// http://unicode.org/reports/tr35/tr35-numbers.html#Operands

// Integer version, simplified

#define UNUSED(x) (void)(x)

static inline uint32_t op_n(int32_t val) { return (uint32_t)(val < 0 ? -val : val); }
static inline uint32_t op_i(uint32_t val) { return val; }
// always zero, when decimal part not exists.
static inline uint32_t op_v(uint32_t val) { UNUSED(val); return 0;}
static inline uint32_t op_w(uint32_t val) { UNUSED(val); return 0; }
static inline uint32_t op_f(uint32_t val) { UNUSED(val); return 0; }
static inline uint32_t op_t(uint32_t val) { UNUSED(val); return 0; }

static lv_i18n_phrase_t en_us_singulars[] = {
    {"ChangeLangWarnText", "The device will restart to change the language."},
    {"Start", "Start"},
    {NULL, NULL} // End mark
};



static uint8_t en_us_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t v = op_v(n); UNUSED(v);

    if ((i == 1 && v == 0)) return LV_I18N_PLURAL_TYPE_ONE;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t en_us_lang = {
    .locale_name = "en-US",
    .singulars = en_us_singulars,

    .locale_plural_fn = en_us_plural_fn
};

static lv_i18n_phrase_t ar_singulars[] = {
    {"ChangeLangWarnText", "بعد تغيير لغة النظام، سيتم إعادة تشغيل الجهاز تلقائياً."},
    {"Start", "开始"},
    {NULL, NULL} // End mark
};



static uint8_t ar_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);

    uint32_t n100 = n % 100;
    if ((n == 0)) return LV_I18N_PLURAL_TYPE_ZERO;
    if ((n == 1)) return LV_I18N_PLURAL_TYPE_ONE;
    if ((n == 2)) return LV_I18N_PLURAL_TYPE_TWO;
    if (((3 <= n100 && n100 <= 10))) return LV_I18N_PLURAL_TYPE_FEW;
    if (((11 <= n100 && n100 <= 99))) return LV_I18N_PLURAL_TYPE_MANY;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t ar_lang = {
    .locale_name = "ar",
    .singulars = ar_singulars,

    .locale_plural_fn = ar_plural_fn
};

static lv_i18n_phrase_t ja_singulars[] = {
    {"Language", "言語"},
    {"Start", "スタート"},
    {"Machine error. Please restart.", "予期せぬエラーが発生しました。再起動してください。"},
    {"If error still occurs after restart, please contact support.", "再起動後もエラーが続く場合は、技術サポートまでご連絡ください。"},
    {"Restart", "再起動"},
    {"ChangeLangWarnText", "システム言語を切り替えると、デバイスは自動的に再起動します。"},
    {"ChangeLangWarnTitle", "システム言語を変更"},
    {"ChangeLangWarnConfirm", "変更"},
    {"ChangeLangWarnCancel", "キャンセル"},
    {"ChangeLangWarnConfirmText", "システム言語を変更しますか？"},
    {"ChangeLangWarnConfirmTitle", "システム言語を変更"},
    {"ChangeLangWarnConfirmConfirm", "変更"},
    {"ChangeLangWarnConfirmCancel", "キャンセル"},
    {"ChangeLangWarnConfirmConfirmText", "システム言語を変更しますか？"},
    {"ChangeLangWarnConfirmConfirmTitle", "システム言語を変更"},
    {"ChangeLangWarnConfirmConfirmConfirm", "変更"},
    {"ChangeLangWarnConfirmConfirmCancel", "キャンセル"},
    {NULL, NULL} // End mark
};



static uint8_t ja_plural_fn(int32_t num)
{



    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t ja_lang = {
    .locale_name = "ja",
    .singulars = ja_singulars,

    .locale_plural_fn = ja_plural_fn
};

static lv_i18n_phrase_t ru_ru_singulars[] = {
    {"Language", "Язык"},
    {"Start", "Старт"},
    {"Machine error. Please restart.", "Ошибка машины. Перезапустите устройство."},
    {"If error still occurs after restart, please contact support.", "Если ошибка не исчезнет после перезагрузки, свяжитесь с технической поддержкой."},
    {"Restart", "Перезагрузка"},
    {"ChangeLangWarnText", "Системный язык изменится после перезагрузки."},
    {NULL, NULL} // End mark
};



static uint8_t ru_ru_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);
    uint32_t v = op_v(n); UNUSED(v);
    uint32_t i = op_i(n); UNUSED(i);
    uint32_t i10 = i % 10;
    uint32_t i100 = i % 100;
    if ((v == 0 && i10 == 1 && i100 != 11)) return LV_I18N_PLURAL_TYPE_ONE;
    if ((v == 0 && (2 <= i10 && i10 <= 4) && (!(12 <= i100 && i100 <= 14)))) return LV_I18N_PLURAL_TYPE_FEW;
    if ((v == 0 && i10 == 0) || (v == 0 && (5 <= i10 && i10 <= 9)) || (v == 0 && (11 <= i100 && i100 <= 14))) return LV_I18N_PLURAL_TYPE_MANY;
    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t ru_ru_lang = {
    .locale_name = "ru-RU",
    .singulars = ru_ru_singulars,

    .locale_plural_fn = ru_ru_plural_fn
};

static lv_i18n_phrase_t zh_cn_singulars[] = {
    {"Language", "语言"},
    {"ChangeLangWarnText", "系统语言切换后设备会自动重启。"},
    {"Start", "开始"},
    {NULL, NULL} // End mark
};



static uint8_t zh_cn_plural_fn(int32_t num)
{



    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t zh_cn_lang = {
    .locale_name = "zh-CN",
    .singulars = zh_cn_singulars,

    .locale_plural_fn = zh_cn_plural_fn
};

const lv_i18n_language_pack_t lv_i18n_language_pack[] = {
    &en_us_lang,
    &ar_lang,
    &ja_lang,
    &ru_ru_lang,
    &zh_cn_lang,
    NULL // End mark
};

////////////////////////////////////////////////////////////////////////////////


// Internal state
static const lv_i18n_language_pack_t * current_lang_pack;
static const lv_i18n_lang_t * current_lang;


/**
 * Reset internal state. For testing.
 */
void __lv_i18n_reset(void)
{
    current_lang_pack = NULL;
    current_lang = NULL;
}

/**
 * Set the languages for internationalization
 * @param langs pointer to the array of languages. (Last element has to be `NULL`)
 */
int lv_i18n_init(const lv_i18n_language_pack_t * langs)
{
    if(langs == NULL) return -1;
    if(langs[0] == NULL) return -1;

    current_lang_pack = langs;
    current_lang = langs[0];     /*Automatically select the first language*/
    return 0;
}

/**
 * Change the localization (language)
 * @param l_name name of the translation locale to use. E.g. "en-GB"
 */
int lv_i18n_set_locale(const char * l_name)
{
    if(current_lang_pack == NULL) return -1;

    uint16_t i;

    for(i = 0; current_lang_pack[i] != NULL; i++) {
        // Found -> finish
        if(strcmp(current_lang_pack[i]->locale_name, l_name) == 0) {
            current_lang = current_lang_pack[i];
            return 0;
        }
    }

    return -1;
}


static const char * __lv_i18n_get_text_core(lv_i18n_phrase_t * trans, const char * msg_id)
{
    uint16_t i;
    for(i = 0; trans[i].msg_id != NULL; i++) {
        if(strcmp(trans[i].msg_id, msg_id) == 0) {
            /*The msg_id has found. Check the translation*/
            if(trans[i].translation) return trans[i].translation;
        }
    }

    return NULL;
}


/**
 * Get the translation from a message ID
 * @param msg_id message ID
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text(const char * msg_id)
{
    if(current_lang == NULL) return msg_id;

    const lv_i18n_lang_t * lang = current_lang;
    const void * txt;

    // Search in current locale
    if(lang->singulars != NULL) {
        txt = __lv_i18n_get_text_core(lang->singulars, msg_id);
        if (txt != NULL) return txt;
    }

    // Try to fallback
    if(lang == current_lang_pack[0]) return msg_id;
    lang = current_lang_pack[0];

    // Repeat search for default locale
    if(lang->singulars != NULL) {
        txt = __lv_i18n_get_text_core(lang->singulars, msg_id);
        if (txt != NULL) return txt;
    }

    return msg_id;
}

/**
 * Get the translation from a message ID and apply the language's plural rule to get correct form
 * @param msg_id message ID
 * @param num an integer to select the correct plural form
 * @return the translation of `msg_id` on the set local
 */
const char * lv_i18n_get_text_plural(const char * msg_id, int32_t num)
{
    if(current_lang == NULL) return msg_id;

    const lv_i18n_lang_t * lang = current_lang;
    const void * txt;
    lv_i18n_plural_type_t ptype;

    // Search in current locale
    if(lang->locale_plural_fn != NULL) {
        ptype = lang->locale_plural_fn(num);

        if(lang->plurals[ptype] != NULL) {
            txt = __lv_i18n_get_text_core(lang->plurals[ptype], msg_id);
            if (txt != NULL) return txt;
        }
    }

    // Try to fallback
    if(lang == current_lang_pack[0]) return msg_id;
    lang = current_lang_pack[0];

    // Repeat search for default locale
    if(lang->locale_plural_fn != NULL) {
        ptype = lang->locale_plural_fn(num);

        if(lang->plurals[ptype] != NULL) {
            txt = __lv_i18n_get_text_core(lang->plurals[ptype], msg_id);
            if (txt != NULL) return txt;
        }
    }

    return msg_id;
}

/**
 * Get the name of the currently used locale.
 * @return name of the currently used locale. E.g. "en-GB"
 */
const char * lv_i18n_get_current_locale(void)
{
    if(!current_lang) return NULL;
    return current_lang->locale_name;
}
