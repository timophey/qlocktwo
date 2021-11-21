class App extends App_Common {
    ticker500 = true;
    weekdays = (locale.weekdays !== undefined) ? locale.weekdays : ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday","Sunday"];

    addr_begin = 0x03;
    addr_end = 0x04;

    getDefaultState(){
        let clientNow = new Date();
        return {
            now: {h: 12, i: 8, s: 57, y: 20, m: 3, d: 17, w: 2},
            t: {h: 0, i: 0, s: 0},
            cd: {
                y: clientNow.getFullYear() % 100,
                m: clientNow.getMonth()+1,
                d: clientNow.getDate(),
                h: clientNow.getHours(),
                i: clientNow.getMinutes(),
                s: clientNow.getSeconds()
            },
            follow: false,
        }
    }
    eventChange(k,v){
        this.state.now[k]=parseInt(v);
        this.setState(this.state);
        sendWS({cmd:"setTime",now:this.state.now});
    }

    sendNow(){
        let devDate = new Date();
        sendWS({
            cmd:"setTime",
            now:{
                y: devDate.getFullYear() % 100,
                m: devDate.getMonth() +1,
                d: devDate.getDate(),
                h: devDate.getHours(),
                i: devDate.getMinutes(),
                s: devDate.getSeconds(),
                w: devDate.getDay()
            }
        });
    }

    render(){
        let clientNow = new Date();
        var cb = this.eventChange.bind(this);
        let ee = this.state.EEPROM;
        // recalc timezone
        let tz = ee[0x04];
        if(tz > 24) tz -= 256;
        var element = <ul className="qlocktwo__letters text-left">
            <li className="pop_header text-right"><a href="/"><span className="icon-house"></span></a></li>
            <li className="i-split">Дата</li>
            <li className="i-split">
                <RangePeaker field="d" value={this.state.now.d} min={1} max={31} cb={cb} w={1}/>
                .
                <RangePeaker field="m" value={this.state.now.m} min={1} max={12} cb={cb} w={1}/>
                .
                <RangePeaker field="y" value={this.state.now.y} min={clientNow.getFullYear() % 100 - 2} max={clientNow.getFullYear() % 100 + 2} cb={cb}/>
            </li>
            <li className="i-split">Время</li>
            <li className="i-split">
                <RangePeaker field="h" value={this.state.now.h} min={0} max={23} cb={cb} w={1}/>
                :
                <RangePeaker field="i" value={this.state.now.i} min={0} max={59} cb={cb} w={1}/>
                :
                <RangePeaker field="s" value={this.state.now.s} min={0} max={59} cb={cb} w={1}/>
            </li>
            <li className="i-split">&nbsp;</li>
            <li className="i-split">Часть света</li>
            <li className="i-split">
                {/*((tz>0)?'+':'-')+(Math.floor(Math.abs(tz/2))+':'+(tz%2) ? '30':'00')*/}
                {/*+((tz%2) ? '30':'00')*/}
                UTC{(tz==0)?'=':((tz>0)?'+':'-')+(Math.floor(Math.abs(tz/2)).toString())+':'+((tz%2) ? '30':'00')}
            </li>
            <li className="i-split">
                <InputRange value={tz} addr={0x04} onUpdateCmd="reloadColors" min={-24} max={24}/>
            </li>
            <li><a className="i-split" onClick={this.sendAction.bind(this,"updatentp",false)}>Обновить</a></li>
            <li className="i-split">&nbsp;</li>
            {(this.state.commited == false)?<li><a className="i-split" onClick={this.sendAction.bind(this,"commit",false)}>Запомнить</a></li>:""}
            <li className="i-split">&nbsp;</li>
            <a onClick={this.sendNow.bind(this)}>
                <li className="i-split">С клиента</li>
                <li className="text-nowrap i-nosplit"><span>{clientNow.toLocaleString()}</span></li>
            </a>
        </ul>
        // element = ;
        // console.log(element)
        // for(var str of element.props.children)
        return this.wrapChildren(element);
        // element;
    }

}



ReactDOM.render(<App/>, document.getElementById("app"));