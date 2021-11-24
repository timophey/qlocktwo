var locale = {
    index_home: "Крониксы - управление",
    index_wifi_simple: "WiFi Подключения",
    index_time_config: "Время",
    index_display_config: "Отображение",
    index_firmware: "Обновление прошивки",
    index_demos: "Демо режимы",
    index_restore: "Резерв и Восстановление настроек",

    action_reboot: "Перезагрузка",
    action_home: "На главную",

    time_current:"Текущее время",
    time_timer:"Обратный отсчёт",
    time_timer_max:"не более 4 дней",
    time_timer_start:"Старт",
    time_timer_stop:"Стоп",
    time_countdown:"Обратный отсчёт до... (установить таймер)",
    time_set:"Установить",
    time_device:"с устройства",
    time_set_start:"...и запустить",
    weekdays: ["Воскресенье", "Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота"],
};
document.addEventListener('DOMContentLoaded',function(){
    document.querySelectorAll('[data-loc]').forEach(function(el,i){
        var loc_key = el.dataset.loc;
        if(loc_key in locale) el.innerText = locale[loc_key];
    });
});
