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
    {"auto_fill_close_tips", "Auto Filling Closed"},
    {"auto_fill_open_tips", "Auto Filling Opened"},
    {"check_fill_mod_tips", "Check Filling Mod!"},
    {"click_cancel", "Cancel"},
    {"click_reboot", "Reboot"},
    {"common_activated", "{} activated"},
    {"common_active", "Active: {}"},
    {"common_auto", "Auto"},
    {"common_bootloader", "Bootloader"},
    {"common_cool", "Cool"},
    {"common_day", "Day"},
    {"common_device_count", "{} devices"},
    {"common_energy_kwh", "{:.1f} kWh"},
    {"common_grams", "{} g"},
    {"common_heat", "Heat"},
    {"common_humidity_percent", "{:.0f}%"},
    {"common_humidity_rh", "{}RH%"},
    {"common_locked", "Locked"},
    {"common_material_kg", "{} ({}kg)"},
    {"common_newton", "{} N"},
    {"common_off", "OFF"},
    {"common_on", "ON"},
    {"common_open", "Open"},
    {"common_opening", "Opening {}"},
    {"common_percent", "{}%"},
    {"common_temperature_c", "{:.1f}C"},
    {"common_temperature_text_c", "{}C"},
    {"common_warm", "Warm"},
    {"common_week", "Week"},
    {"electron_stack", "Pulsing Release Module"},
    {"empty_info", "--"},
    {"heating_scraper", "Heating Scraper"},
    {"home_alarm_disarmed", "Alarm Disarmed"},
    {"home_all_doors_locked", "All doors locked"},
    {"home_all_lights_off", "All lights off"},
    {"home_away", "Away"},
    {"home_away_mode", "Away mode"},
    {"home_cameras_active", "Cameras Active"},
    {"home_ceiling_light", "Ceiling Light"},
    {"home_climate", "Climate"},
    {"home_doors_locked", "Doors Locked"},
    {"home_floor_lamp", "Floor Lamp"},
    {"home_front_door", "Front Door"},
    {"home_garage_door", "Garage Door"},
    {"home_hallway_no_motion", "Hallway - No motion"},
    {"home_lights", "Lights"},
    {"home_lights_off", "Lights Off"},
    {"home_lock_all", "Lock All"},
    {"home_motion_sensor", "Motion Sensor"},
    {"home_security", "Security"},
    {"home_title", "Smart Home"},
    {"home_today", "Today"},
    {"home_weather", "Sunny  72F  45%"},
    {"language", "Language"},
    {"language_arabic", "Arabic"},
    {"language_chinese", "Chinese"},
    {"language_english", "English"},
    {"language_japanese", "Japanese"},
    {"language_russian", "Russian"},
    {"lcd_screen_label", "LCD Screen"},
    {"mod_used", "Worked"},
    {"nav_devices", "Devices"},
    {"nav_home", "Home"},
    {"nav_rooms", "Rooms"},
    {"nav_scenes", "Scenes"},
    {"nav_settings", "Settings"},
    {"net_disconnected", "Disconnected"},
    {"panel_autofill_label", "Automatic Resin Fill"},
    {"panel_pulse_used", "Release film service life"},
    {"panel_resin_remaining", "Remaining:"},
    {"please_replace", "Please replace"},
    {"pulsing_release_resin_tank", "Pulsing release resin tank"},
    {"regular_resin_tank", "Regular resin tank"},
    {"replace_material_tray_tips", "Please replace the resin tank in time."},
    {"restart_tips", "System Restart"},
    {"room_bathroom", "Bathroom"},
    {"room_bedroom", "Bedroom"},
    {"room_garage", "Garage"},
    {"room_hallway", "Hallway"},
    {"room_icon_bath", "Bath"},
    {"room_icon_bed", "Bed"},
    {"room_icon_car", "Car"},
    {"room_icon_cook", "Cook"},
    {"room_icon_door", "Door"},
    {"room_icon_sofa", "Sofa"},
    {"room_kitchen", "Kitchen"},
    {"room_living_room", "Living Room"},
    {"scene_away_mode", "Away Mode"},
    {"scene_away_mode_desc", "All off, cameras on"},
    {"scene_good_morning", "Good Morning"},
    {"scene_good_morning_desc", "Lights on, blinds open"},
    {"scene_good_night", "Good Night"},
    {"scene_good_night_desc", "All off, doors locked"},
    {"scene_movie_time", "Movie Time"},
    {"scene_movie_time_desc", "Lights dim, TV on"},
    {"scenes_active", "Active: None"},
    {"scraper_normal", "Regular scraper"},
    {"scraper_title_name", "Scraper"},
    {"sensor_curve_stats", "Value     Max   {}      Min  {}"},
    {"sensor_show_curve", "Show Curve"},
    {"settings_about", "About"},
    {"settings_appearance", "Appearance"},
    {"settings_brightness", "Brightness"},
    {"settings_language_apply", "Apply"},
    {"settings_network", "Network"},
    {"settings_notifications", "Notifications"},
    {"settings_push_notifications", "Push Notifications"},
    {"settings_theme", "Theme"},
    {"settings_theme_auto", "Auto"},
    {"settings_theme_dark", "Dark"},
    {"settings_theme_light", "Light"},
    {"settings_view_license", "View License"},
    {"settings_wifi", "WiFi"},
    {"status_abnormal", "Abnormal"},
    {"status_error", "Error"},
    {"status_normal", "Normal"},
    {"status_uninstalled", "Uninstalled"},
    {"thermostat_target", "Target: {:.1f}C"},
    {"wearing_out", "Wearing out"},
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
    {"auto_fill_close_tips", "تم إيقاف التعبئة التلقائية"},
    {"auto_fill_open_tips", "تم بدء التعبئة التلقائية"},
    {"check_fill_mod_tips", "تحقق من وحدة التعبئة!"},
    {"click_cancel", "إلغاء"},
    {"click_reboot", "إعادة التشغيل"},
    {"common_activated", "تم تفعيل {}"},
    {"common_active", "النشط: {}"},
    {"common_auto", "تلقائي"},
    {"common_bootloader", "محمل الإقلاع"},
    {"common_cool", "تبريد"},
    {"common_day", "نهار"},
    {"common_device_count", "{} أجهزة"},
    {"common_energy_kwh", "{:.1f} كيلوواط ساعة"},
    {"common_grams", "{} غ"},
    {"common_heat", "تدفئة"},
    {"common_humidity_percent", "{:.0f}%"},
    {"common_humidity_rh", "{}RH%"},
    {"common_locked", "مقفل"},
    {"common_material_kg", "{} ({}كغ)"},
    {"common_newton", "{} N"},
    {"common_off", "إيقاف"},
    {"common_on", "تشغيل"},
    {"common_open", "مفتوح"},
    {"common_opening", "جارٍ فتح {}"},
    {"common_percent", "{}%"},
    {"common_temperature_c", "{:.1f}°C"},
    {"common_temperature_text_c", "{}°C"},
    {"common_warm", "دافئ"},
    {"common_week", "أسبوع"},
    {"electron_stack", "وحدة تجريد النبض"},
    {"empty_info", "--"},
    {"heating_scraper", "مكشطة التسخين"},
    {"home_alarm_disarmed", "الإنذار معطل"},
    {"home_all_doors_locked", "تم قفل جميع الأبواب"},
    {"home_all_lights_off", "تم إطفاء جميع الأنوار"},
    {"home_away", "بعيد"},
    {"home_away_mode", "وضع البعيد"},
    {"home_cameras_active", "الكاميرات نشطة"},
    {"home_ceiling_light", "ضوء السقف"},
    {"home_climate", "المناخ"},
    {"home_doors_locked", "الأبواب مقفلة"},
    {"home_floor_lamp", "مصباح أرضي"},
    {"home_front_door", "الباب الأمامي"},
    {"home_garage_door", "باب المرآب"},
    {"home_hallway_no_motion", "الممر - لا حركة"},
    {"home_lights", "الأنوار"},
    {"home_lights_off", "إطفاء الأنوار"},
    {"home_lock_all", "قفل الكل"},
    {"home_motion_sensor", "مستشعر الحركة"},
    {"home_security", "الأمان"},
    {"home_title", "المنزل الذكي"},
    {"home_today", "اليوم"},
    {"home_weather", "مشمس  72F  45%"},
    {"language", "لغة"},
    {"language_arabic", "العربية"},
    {"language_chinese", "الصينية"},
    {"language_english", "الإنجليزية"},
    {"language_japanese", "اليابانية"},
    {"language_russian", "الروسية"},
    {"lcd_screen_label", "شاشة LCD"},
    {"mod_used", "مستخدم"},
    {"nav_devices", "الأجهزة"},
    {"nav_home", "الرئيسية"},
    {"nav_rooms", "الغرف"},
    {"nav_scenes", "المشاهد"},
    {"nav_settings", "الإعدادات"},
    {"net_disconnected", "غير متصل"},
    {"panel_autofill_label", "التعبئة التلقائية للراتنج"},
    {"panel_pulse_used", "عمر خدمة فيلم الإطلاق"},
    {"panel_resin_remaining", "المتبقي:"},
    {"please_replace", "يرجى الاستبدال"},
    {"pulsing_release_resin_tank", "خزان راتنج التجريد النبضي"},
    {"regular_resin_tank", "خزان راتنج عادي"},
    {"replace_material_tray_tips", "يرجى استبدال خزان الراتنج في الوقت المناسب."},
    {"restart_tips", "إعادة تشغيل النظام"},
    {"room_bathroom", "الحمام"},
    {"room_bedroom", "غرفة النوم"},
    {"room_garage", "المرآب"},
    {"room_hallway", "الممر"},
    {"room_icon_bath", "حوض"},
    {"room_icon_bed", "سرير"},
    {"room_icon_car", "سيارة"},
    {"room_icon_cook", "طبخ"},
    {"room_icon_door", "باب"},
    {"room_icon_sofa", "أريكة"},
    {"room_kitchen", "المطبخ"},
    {"room_living_room", "غرفة المعيشة"},
    {"scene_away_mode", "وضع البعيد"},
    {"scene_away_mode_desc", "إيقاف الكل، تشغيل الكاميرات"},
    {"scene_good_morning", "صباح الخير"},
    {"scene_good_morning_desc", "الأنوار مضاءة، الستائر مفتوحة"},
    {"scene_good_night", "تصبح على خير"},
    {"scene_good_night_desc", "إيقاف الكل، قفل الأبواب"},
    {"scene_movie_time", "وقت الفيلم"},
    {"scene_movie_time_desc", "إضاءة خافتة، تشغيل التلفاز"},
    {"scenes_active", "النشط: لا شيء"},
    {"scraper_normal", "مكشطة عادية"},
    {"scraper_title_name", "مكشطة"},
    {"sensor_curve_stats", "القيمة     الأقصى   {}      الأدنى  {}"},
    {"sensor_show_curve", "عرض المنحنى"},
    {"settings_about", "حول"},
    {"settings_appearance", "المظهر"},
    {"settings_brightness", "السطوع"},
    {"settings_language_apply", "تطبيق"},
    {"settings_network", "الشبكة"},
    {"settings_notifications", "الإشعارات"},
    {"settings_push_notifications", "إشعارات الدفع"},
    {"settings_theme", "السمة"},
    {"settings_theme_auto", "تلقائي"},
    {"settings_theme_dark", "داكن"},
    {"settings_theme_light", "فاتح"},
    {"settings_view_license", "عرض الترخيص"},
    {"settings_wifi", "Wi-Fi"},
    {"status_abnormal", "غير طبيعي"},
    {"status_error", "خطأ"},
    {"status_normal", "طبيعي"},
    {"status_uninstalled", "غير مثبت"},
    {"thermostat_target", "الهدف: {:.1f}°C"},
    {"wearing_out", "تآكل"},
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

static lv_i18n_phrase_t ja_jp_singulars[] = {
    {"auto_fill_close_tips", "自動充填が停止しました"},
    {"auto_fill_open_tips", "自動充填を開始しました"},
    {"check_fill_mod_tips", "充填モジュールを確認してください！"},
    {"click_cancel", "キャンセル"},
    {"click_reboot", "再起動"},
    {"common_activated", "{} を有効にしました"},
    {"common_active", "アクティブ: {}"},
    {"common_auto", "自動"},
    {"common_bootloader", "ブートローダー"},
    {"common_cool", "冷房"},
    {"common_day", "日"},
    {"common_device_count", "{} 台のデバイス"},
    {"common_energy_kwh", "{:.1f} kWh"},
    {"common_grams", "{} g"},
    {"common_heat", "暖房"},
    {"common_humidity_percent", "{:.0f}%"},
    {"common_humidity_rh", "{}RH%"},
    {"common_locked", "ロック済み"},
    {"common_material_kg", "{} ({}kg)"},
    {"common_newton", "{} N"},
    {"common_off", "オフ"},
    {"common_on", "オン"},
    {"common_open", "開"},
    {"common_opening", "{} を開いています"},
    {"common_percent", "{}%"},
    {"common_temperature_c", "{:.1f}℃"},
    {"common_temperature_text_c", "{}℃"},
    {"common_warm", "暖色"},
    {"common_week", "週"},
    {"electron_stack", "パルス剥離モジュール"},
    {"empty_info", "--"},
    {"heating_scraper", "加熱スクレーパー"},
    {"home_alarm_disarmed", "警報解除"},
    {"home_all_doors_locked", "すべてのドアをロックしました"},
    {"home_all_lights_off", "すべての照明を消しました"},
    {"home_away", "外出"},
    {"home_away_mode", "外出モード"},
    {"home_cameras_active", "カメラ作動中"},
    {"home_ceiling_light", "シーリングライト"},
    {"home_climate", "空調"},
    {"home_doors_locked", "ドアロック済み"},
    {"home_floor_lamp", "フロアランプ"},
    {"home_front_door", "玄関ドア"},
    {"home_garage_door", "ガレージドア"},
    {"home_hallway_no_motion", "廊下 - 動きなし"},
    {"home_lights", "照明"},
    {"home_lights_off", "照明オフ"},
    {"home_lock_all", "すべてロック"},
    {"home_motion_sensor", "モーションセンサー"},
    {"home_security", "セキュリティ"},
    {"home_title", "スマートホーム"},
    {"home_today", "今日"},
    {"home_weather", "晴れ  72F  45%"},
    {"language", "言語"},
    {"language_arabic", "アラビア語"},
    {"language_chinese", "簡体字中国語"},
    {"language_english", "英語"},
    {"language_japanese", "日本語"},
    {"language_russian", "ロシア語"},
    {"lcd_screen_label", "LCD画面"},
    {"mod_used", "使用済み"},
    {"nav_devices", "デバイス"},
    {"nav_home", "ホーム"},
    {"nav_rooms", "部屋"},
    {"nav_scenes", "シーン"},
    {"nav_settings", "設定"},
    {"net_disconnected", "未接続"},
    {"panel_autofill_label", "Automatic Resin Fill"},
    {"panel_pulse_used", "Release film service life"},
    {"panel_resin_remaining", "Remaining:"},
    {"please_replace", "交換してください"},
    {"pulsing_release_resin_tank", "パルス剥離用樹脂タンク"},
    {"regular_resin_tank", "Regular resin tank"},
    {"replace_material_tray_tips", "樹脂タンクを早めに交換してください。"},
    {"restart_tips", "システムを再起動"},
    {"room_bathroom", "浴室"},
    {"room_bedroom", "寝室"},
    {"room_garage", "ガレージ"},
    {"room_hallway", "廊下"},
    {"room_icon_bath", "風呂"},
    {"room_icon_bed", "ベッド"},
    {"room_icon_car", "車"},
    {"room_icon_cook", "料理"},
    {"room_icon_door", "ドア"},
    {"room_icon_sofa", "ソファ"},
    {"room_kitchen", "キッチン"},
    {"room_living_room", "リビング"},
    {"scene_away_mode", "外出モード"},
    {"scene_away_mode_desc", "すべてオフ、カメラオン"},
    {"scene_good_morning", "おはよう"},
    {"scene_good_morning_desc", "照明オン、ブラインドを開く"},
    {"scene_good_night", "おやすみ"},
    {"scene_good_night_desc", "すべてオフ、ドアロック"},
    {"scene_movie_time", "映画タイム"},
    {"scene_movie_time_desc", "照明を暗く、テレビオン"},
    {"scenes_active", "アクティブ: なし"},
    {"scraper_normal", "通常スクレーパー"},
    {"scraper_title_name", "スクレーパー"},
    {"sensor_curve_stats", "値     最大   {}      最小  {}"},
    {"sensor_show_curve", "曲線を表示"},
    {"settings_about", "について"},
    {"settings_appearance", "外観"},
    {"settings_brightness", "明るさ"},
    {"settings_language_apply", "適用"},
    {"settings_network", "ネットワーク"},
    {"settings_notifications", "通知"},
    {"settings_push_notifications", "プッシュ通知"},
    {"settings_theme", "テーマ"},
    {"settings_theme_auto", "自動"},
    {"settings_theme_dark", "ダーク"},
    {"settings_theme_light", "ライト"},
    {"settings_view_license", "ライセンスを表示"},
    {"settings_wifi", "Wi-Fi"},
    {"status_abnormal", "異常"},
    {"status_error", "故障"},
    {"status_normal", "Normal"},
    {"status_uninstalled", "Uninstalled"},
    {"thermostat_target", "目標: {:.1f}℃"},
    {"wearing_out", "摩耗中"},
    {NULL, NULL} // End mark
};



static uint8_t ja_jp_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);

    return LV_I18N_PLURAL_TYPE_OTHER;
}

static const lv_i18n_lang_t ja_jp_lang = {
    .locale_name = "ja-JP",
    .singulars = ja_jp_singulars,

    .locale_plural_fn = ja_jp_plural_fn
};

static lv_i18n_phrase_t ru_ru_singulars[] = {
    {"auto_fill_close_tips", "Автозаполнение остановлено"},
    {"auto_fill_open_tips", "Автозаполнение запущено"},
    {"check_fill_mod_tips", "Проверьте модуль заполнения!"},
    {"click_cancel", "Отмена"},
    {"click_reboot", "Перезагрузить"},
    {"common_activated", "{} активирован"},
    {"common_active", "Активно: {}"},
    {"common_auto", "Авто"},
    {"common_bootloader", "Загрузчик"},
    {"common_cool", "Охлаждение"},
    {"common_day", "День"},
    {"common_device_count", "Устройств: {}"},
    {"common_energy_kwh", "{:.1f} кВт·ч"},
    {"common_grams", "{} г"},
    {"common_heat", "Нагрев"},
    {"common_humidity_percent", "{:.0f}%"},
    {"common_humidity_rh", "{}RH%"},
    {"common_locked", "Заблокировано"},
    {"common_material_kg", "{} ({}кг)"},
    {"common_newton", "{} Н"},
    {"common_off", "ВЫКЛ"},
    {"common_on", "ВКЛ"},
    {"common_open", "Открыто"},
    {"common_opening", "Открытие {}"},
    {"common_percent", "{}%"},
    {"common_temperature_c", "{:.1f}°C"},
    {"common_temperature_text_c", "{}°C"},
    {"common_warm", "Тёплый"},
    {"common_week", "Неделя"},
    {"electron_stack", "Модуль импульсного отделения"},
    {"empty_info", "--"},
    {"heating_scraper", "Нагревательный скребок"},
    {"home_alarm_disarmed", "Сигнализация отключена"},
    {"home_all_doors_locked", "Все двери заблокированы"},
    {"home_all_lights_off", "Все огни выключены"},
    {"home_away", "Не дома"},
    {"home_away_mode", "Режим «Не дома»"},
    {"home_cameras_active", "Камеры активны"},
    {"home_ceiling_light", "Потолочный светильник"},
    {"home_climate", "Климат"},
    {"home_doors_locked", "Двери заблокированы"},
    {"home_floor_lamp", "Торшер"},
    {"home_front_door", "Входная дверь"},
    {"home_garage_door", "Дверь гаража"},
    {"home_hallway_no_motion", "Коридор — нет движения"},
    {"home_lights", "Освещение"},
    {"home_lights_off", "Выключить свет"},
    {"home_lock_all", "Заблокировать всё"},
    {"home_motion_sensor", "Датчик движения"},
    {"home_security", "Безопасность"},
    {"home_title", "Умный дом"},
    {"home_today", "Сегодня"},
    {"home_weather", "Солнечно  72F  45%"},
    {"language", "Язык"},
    {"language_arabic", "Арабский"},
    {"language_chinese", "Китайский (упрощенный)"},
    {"language_english", "Английский"},
    {"language_japanese", "Японский"},
    {"language_russian", "Русский"},
    {"lcd_screen_label", "ЖК-экран"},
    {"mod_used", "Использовано"},
    {"nav_devices", "Устройства"},
    {"nav_home", "Главная"},
    {"nav_rooms", "Комнаты"},
    {"nav_scenes", "Сцены"},
    {"nav_settings", "Настройки"},
    {"net_disconnected", "Отключено"},
    {"panel_autofill_label", "Automatic Resin Fill"},
    {"panel_pulse_used", "Release film service life"},
    {"panel_resin_remaining", "Remaining:"},
    {"please_replace", "Пожалуйста, замените"},
    {"pulsing_release_resin_tank", "Бак для смолы импульсного отделения"},
    {"regular_resin_tank", "Regular resin tank"},
    {"replace_material_tray_tips", "Пожалуйста, своевременно замените бак для смолы."},
    {"restart_tips", "Перезагрузить систему"},
    {"room_bathroom", "Ванная"},
    {"room_bedroom", "Спальня"},
    {"room_garage", "Гараж"},
    {"room_hallway", "Коридор"},
    {"room_icon_bath", "Ванна"},
    {"room_icon_bed", "Кровать"},
    {"room_icon_car", "Машина"},
    {"room_icon_cook", "Готовка"},
    {"room_icon_door", "Дверь"},
    {"room_icon_sofa", "Диван"},
    {"room_kitchen", "Кухня"},
    {"room_living_room", "Гостиная"},
    {"scene_away_mode", "Режим «Не дома»"},
    {"scene_away_mode_desc", "Всё выключено, камеры включены"},
    {"scene_good_morning", "Доброе утро"},
    {"scene_good_morning_desc", "Свет включён, жалюзи открыты"},
    {"scene_good_night", "Доброй ночи"},
    {"scene_good_night_desc", "Всё выключено, двери заблокированы"},
    {"scene_movie_time", "Время кино"},
    {"scene_movie_time_desc", "Свет приглушён, ТВ включён"},
    {"scenes_active", "Активно: нет"},
    {"scraper_normal", "Обычный скребок"},
    {"scraper_title_name", "Скребок"},
    {"sensor_curve_stats", "Значение     Макс   {}      Мин  {}"},
    {"sensor_show_curve", "Показать кривую"},
    {"settings_about", "О программе"},
    {"settings_appearance", "Внешний вид"},
    {"settings_brightness", "Яркость"},
    {"settings_language_apply", "Применить"},
    {"settings_network", "Сеть"},
    {"settings_notifications", "Уведомления"},
    {"settings_push_notifications", "Push-уведомления"},
    {"settings_theme", "Тема"},
    {"settings_theme_auto", "Авто"},
    {"settings_theme_dark", "Тёмная"},
    {"settings_theme_light", "Светлая"},
    {"settings_view_license", "Просмотреть лицензию"},
    {"settings_wifi", "Wi-Fi"},
    {"status_abnormal", "Аномалия"},
    {"status_error", "Ошибка"},
    {"status_normal", "Normal"},
    {"status_uninstalled", "Uninstalled"},
    {"thermostat_target", "Цель: {:.1f}°C"},
    {"wearing_out", "Износ"},
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
    {"auto_fill_close_tips", "自动加液已关闭"},
    {"auto_fill_open_tips", "自动加液已开启"},
    {"check_fill_mod_tips", "请检查加液模块"},
    {"click_cancel", "取消"},
    {"click_reboot", "重启"},
    {"common_activated", "{} 已激活"},
    {"common_active", "当前：{}"},
    {"common_auto", "自动"},
    {"common_bootloader", "Bootloader"},
    {"common_cool", "制冷"},
    {"common_day", "日光"},
    {"common_device_count", "{} 个设备"},
    {"common_energy_kwh", "{:.1f} kWh"},
    {"common_grams", "{} g"},
    {"common_heat", "制热"},
    {"common_humidity_percent", "{:.0f}%"},
    {"common_humidity_rh", "{}RH%"},
    {"common_locked", "已上锁"},
    {"common_material_kg", "{} ({}kg)"},
    {"common_newton", "{} N"},
    {"common_off", "关"},
    {"common_on", "开"},
    {"common_open", "打开"},
    {"common_opening", "正在打开 {}"},
    {"common_percent", "{}%"},
    {"common_temperature_c", "{:.1f}℃"},
    {"common_temperature_text_c", "{}℃"},
    {"common_warm", "暖光"},
    {"common_week", "周"},
    {"electron_stack", "电晾衣架"},
    {"empty_info", "--"},
    {"heating_scraper", "空调"},
    {"home_alarm_disarmed", "警报已解除"},
    {"home_all_doors_locked", "所有门已上锁"},
    {"home_all_lights_off", "所有灯已关闭"},
    {"home_away", "离家"},
    {"home_away_mode", "离家模式"},
    {"home_cameras_active", "摄像头已开启"},
    {"home_ceiling_light", "吸顶灯"},
    {"home_climate", "温控"},
    {"home_doors_locked", "门已上锁"},
    {"home_floor_lamp", "落地灯"},
    {"home_front_door", "前门"},
    {"home_garage_door", "车库门"},
    {"home_hallway_no_motion", "走廊 - 无活动"},
    {"home_lights", "灯光"},
    {"home_lights_off", "关灯"},
    {"home_lock_all", "全部上锁"},
    {"home_motion_sensor", "人体传感器"},
    {"home_security", "安防"},
    {"home_title", "智能家居"},
    {"home_today", "今天"},
    {"home_weather", "晴  72F  45%"},
    {"language", "语言"},
    {"language_arabic", "阿拉伯语"},
    {"language_chinese", "中文"},
    {"language_english", "英文"},
    {"language_japanese", "日语"},
    {"language_russian", "俄语"},
    {"lcd_screen_label", "客厅灯光"},
    {"mod_used", "已使用"},
    {"nav_devices", "设备"},
    {"nav_home", "首页"},
    {"nav_rooms", "房间"},
    {"nav_scenes", "场景"},
    {"nav_settings", "设置"},
    {"net_disconnected", "未连接"},
    {"panel_autofill_label", "自动补料"},
    {"panel_pulse_used", "离型膜使用寿命"},
    {"panel_resin_remaining", "剩余："},
    {"please_replace", "尽快更换"},
    {"pulsing_release_resin_tank", "脉冲剥离料盘"},
    {"regular_resin_tank", "普通料盘"},
    {"replace_material_tray_tips", "请及时更换料盘。"},
    {"restart_tips", "重启系统"},
    {"room_bathroom", "浴室"},
    {"room_bedroom", "卧室"},
    {"room_garage", "车库"},
    {"room_hallway", "走廊"},
    {"room_icon_bath", "浴缸"},
    {"room_icon_bed", "床"},
    {"room_icon_car", "车"},
    {"room_icon_cook", "烹饪"},
    {"room_icon_door", "门"},
    {"room_icon_sofa", "沙发"},
    {"room_kitchen", "厨房"},
    {"room_living_room", "客厅"},
    {"scene_away_mode", "离家模式"},
    {"scene_away_mode_desc", "全部关闭，摄像头开启"},
    {"scene_good_morning", "早安"},
    {"scene_good_morning_desc", "开灯，打开窗帘"},
    {"scene_good_night", "晚安"},
    {"scene_good_night_desc", "全部关闭，门上锁"},
    {"scene_movie_time", "观影时间"},
    {"scene_movie_time_desc", "灯光调暗，电视开启"},
    {"scenes_active", "当前：无"},
    {"scraper_normal", "常规刮刀"},
    {"scraper_title_name", "刮刀"},
    {"sensor_curve_stats", "数值     最大   {}      最小  {}"},
    {"sensor_show_curve", "开始显示曲线"},
    {"settings_about", "关于"},
    {"settings_appearance", "外观"},
    {"settings_brightness", "亮度"},
    {"settings_language_apply", "应用"},
    {"settings_network", "网络"},
    {"settings_notifications", "通知"},
    {"settings_push_notifications", "推送通知"},
    {"settings_theme", "主题"},
    {"settings_theme_auto", "自动"},
    {"settings_theme_dark", "深色"},
    {"settings_theme_light", "浅色"},
    {"settings_view_license", "查看许可"},
    {"settings_wifi", "WiFi"},
    {"status_abnormal", "异常"},
    {"status_error", "故障"},
    {"status_normal", "正常"},
    {"status_uninstalled", "未安装"},
    {"thermostat_target", "目标: {:.1f}℃"},
    {"wearing_out", "即将耗尽"},
    {NULL, NULL} // End mark
};



static uint8_t zh_cn_plural_fn(int32_t num)
{
    uint32_t n = op_n(num); UNUSED(n);

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
    &ja_jp_lang,
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
