class App_Timers extends App_Common {

    addr_begin = 0x15;
    addr_end = 0x17;

    render() {
        let ee = this.state.EEPROM;
        return <ul className="qlocktwo__letters text-left">
            <li className="pop_header text-right">
                <a href="/"><span className="icon-house"></span></a>
            </li>
            <li className="i-split">
                <label>Задержка ЧМ: {ee[0x15]/10} сек</label>
                <InputRange value={ee[0x15]} addr={0x15} onUpdateCmd="reloadColors" min={20} max={250}/>
            </li>
            <li className="i-split">
                <label>Задержка печати: {ee[0x16]/1000} сек</label>
                <InputRange value={ee[0x16]} addr={0x16} onUpdateCmd="reloadColors" min={5} max={250}/>
            </li>
            <li className="i-split">
                <label>Таймер мерцания: {ee[0x17]/1000} сек</label>
                <InputRange value={ee[0x17]} addr={0x17} onUpdateCmd="reloadColors" min={5} max={250}/>
            </li>
            {(this.state.commited == false)?[<li>&nbsp;</li>,<li><a className="i-split" onClick={this.sendAction.bind(this,"commit",false)}>Запомнить</a></li>]:""}
        </ul>
    }

}