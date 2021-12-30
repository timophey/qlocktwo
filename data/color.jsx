class App_Colors extends App_Common{

    addr_begin = 0x00;
    addr_end = 0x1A;

    updateHandler(e){
        document.body.style.backgroundColor = e.hexString;
        sendWS({cmd:"setColor",value:e.rgb});
        let e_hsv = e.hsv;
        e_hsv.h*=255/360;
        e_hsv.s*=255/100;
        e_hsv.v*=255/100;
        sendWS({cmd:"setHsv",value:e_hsv});

        let ee = this.state.EEPROM;
        // console.log(e);
        ee[0x0D] = e_hsv.h;
        ee[0x0E] = e_hsv.s;
        ee[0x0F] = e_hsv.v;
        ee[0x10] = e.red;
        ee[0x11] = e.green;
        ee[0x12] = e.blue;
        this.setState({EEPROM:ee});
        // this.setState({hsv:e.hsv});
    }

    updateLeadingHandler(e){
        let e_hsv = e.hsv;
        sendWS({cmd:"setEEPROM",addr:0x18,value:e.red});
        sendWS({cmd:"setEEPROM",addr:0x19,value:e.green});
        sendWS({cmd:"setEEPROM",addr:0x1A,value:e.blue});
        sendWS({cmd:"setEEPROM",addr:0x1B,value:e_hsv.h});
        sendWS({cmd:"reloadColors"});
    }

    // update

    render(){
        let ee = this.state.EEPROM;

        let byte_conf_keys = {
            0: 'use HSV',
            1: 'delta < Value',
            2: 'delta > Value',
            3: 'fall to 0',
            4: 'fall to 255',
            5: 'bounce',
            6: 'use Candle',
            7: 'random Hue',
        }

        let byte_conf2keys = {
            0: 'Leading One',
        }

        let byte_conf1bin = parseInt(ee[0x03]).toString(2);
        let byte_conf1 = "0".repeat(8 - byte_conf1bin.length) + byte_conf1bin;
        console.log('byte_conf1',byte_conf1);

        let byte_conf2bin = parseInt(ee[0x05]).toString(2);
        let byte_conf2 = "0".repeat(8 - byte_conf2bin.length) + byte_conf2bin;
        console.log('byte_conf2',byte_conf2);

        if(byte_conf1[1]==0){
            delete byte_conf_keys[1];
            delete byte_conf_keys[2]
            delete byte_conf_keys[3]
            delete byte_conf_keys[4];
            delete byte_conf_keys[5];
        }

        return <ul className="qlocktwo__letters text-left">
            <li className="pop_header text-right">
                <a href="/"><span className="icon-house"></span></a>
            </li>
            <li className="i-split">Таймеры</li>
            <li className="i-split">
                <label>Таймер ЧМ: {ee[0x15]/10} сек</label>
                <InputRange value={ee[0x15]} addr={0x15} onUpdateCmd="reloadColors" min={20} max={250}/>
            </li>
            <li className="i-split">
                <label>Таймер печати: {ee[0x16]/1000} сек</label>
                <InputRange value={ee[0x16]} addr={0x16} onUpdateCmd="reloadColors" min={5} max={250}/>
            </li>
            <li>&nbsp;</li>

            <li className="i-split">
                <label data-random={byte_conf1[0]=="1" && byte_conf1[7]=="1"} class="pickerHolder">Основной цвет: <InputColorPicker
                    id="main"
                    hsv={{h: ee[0x0D], s: ee[0x0E], v: ee[0x0F]}}
                    value={{r: ee[0x10], g: ee[0x11], b: ee[0x12]}}
                    onUpdate={this.updateHandler.bind(this)}
                /></label>
            </li>
            <li>
                <Flags keys={byte_conf_keys} value={ee[0x03]} addr={0x03} onUpdateCmd="reloadColors"/>
                <Flags keys={byte_conf2keys} value={ee[0x05]} addr={0x05} onUpdateCmd="reloadColors"/>
            </li>
            <li>
                <label data-hidden={byte_conf2[7]==0}>Leading Pixel: <InputColorPicker
                    id="lead"
                    value={{r: ee[0x18], g: ee[0x19], b: ee[0x1A]}}
                    onUpdate={this.updateLeadingHandler.bind(this)}
                /></label>
            </li>
            {/* <li className="i-split">Оттенок</li> */}
            <li className="i-split" data-hidden={byte_conf1[1]==0}>
                <label>Candle dx max: {ee[0x13]}</label>
                <InputRange value={ee[0x13]} addr={0x13} onUpdateCmd="reloadColors"/>
            </li>
            <li className="i-split" data-hidden={byte_conf1[1]==0}>
                <label>Candle dx step: {ee[0x14]}</label>
                <InputRange value={ee[0x14]} addr={0x14} onUpdateCmd="reloadColors" max={64}/>
            </li>
            <li className="i-split" data-hidden={byte_conf1[1]==0}>
                <label>Candle timer: {ee[0x17]/1000} сек</label>
                <InputRange value={ee[0x17]} addr={0x17} onUpdateCmd="reloadColors" min={5} max={250}/>
            </li>
            {(this.state.commited == false)?[<li>&nbsp;</li>,<li><a className="i-split" onClick={this.sendAction.bind(this,"commit",false)}>Запомнить</a></li>]:""}
            <style>
            
            </style>
        </ul>
    }

}

class InputColorPicker extends Component_Common{

    colorPicker;
    colorPickerNode = document.createElement("div");
    ref = {};
    onUpdate(){};

    setRef = (ref) => {
        this.ref = ref;
    };

    constructor(props) {
        super(props);
        this.state = this.getDefaultState(props);
        if(!!props.onUpdate) this.onUpdate = props.onUpdate;
    }

    componentWillReceiveProps(nextProps){
        this.setState({
            rgb: nextProps.value,
            hsv: nextProps.hsv,
        });
    }

    getDefaultState(props){
        // console.log('getDefaultState',props);
        return {
            opened: false,
            popup: false,
            rgb: props.value,
            hsv:{h:0,s:0,v:0}
        };
    }

    togglePopup(){
        this.state.popup = !this.state.popup;
        if(!this.state.popup) this.colorPicker = null;
        this.setState(this.state);
        getNow();
    }

    render(){
        var v = this.state.rgb;
        var c = "rgb("+v.r+","+v.g+","+v.b+")";
        console.log('c',this.state);
        let block = <div
            style={{display:"inline-block",backgroundColor:c,height:"1em",width:"1em",verticalAlign: "middle",border: "2px solid #fff"}}
            onClick={this.togglePopup.bind(this)}
            className="preview"
        ></div>;

        if(this.state.popup){ //
            // console.log('render popup picker');
            // console.log(this.props);
            let popup = <ul className="qlocktwo__letters text-left">
                <li className="pop_header text-right"><a onClick={this.togglePopup.bind(this)}>x</a></li>
                <li className="i-split">Цвет</li>
                <li>&nbsp;</li>
                <li>
                    <div id={"picker_"+this.props.id} className="d-flex justify-content-center"></div>
                </li>
                <li>&nbsp;</li>
                <li>
                    CRGB( {v.r} , {v.g} , {v.b} );
                </li>
                {(!!this.state.hsv)?<li>CHSV( {Math.floor(this.state.hsv.h)} , {Math.floor(this.state.hsv.s)} , {Math.floor(this.state.hsv.v)} );</li>:""}
                <li>&nbsp;</li>
                {(this.state.commited == false)?<li><a className="i-split" onClick={this.sendAction.bind(this,"commit",false)}>Запомнить</a></li>:""}
                {/**/}
            </ul>

            if( !!document.getElementById("picker_"+this.props.id) ){
                if( !this.colorPicker ){
                    this.colorPicker = new iro.ColorPicker("#picker_"+this.props.id,{
                        color: c,
                        width: 270,
                        layout:[
                            {
                                component: iro.ui.Wheel,
                            },{
                                component: iro.ui.Slider,
                                options: {sliderType: 'red',},
                            },{
                                component: iro.ui.Slider,
                                options: {sliderType: 'green',},
                            },{
                                component: iro.ui.Slider,
                                options: {sliderType: 'blue',},
                            },
                            {
                                component: iro.ui.Box,
                            },
                            {
                                component: iro.ui.Slider,
                                options: {sliderType: 'hue'},
                            },
                            {
                                component: iro.ui.Slider,
                                options: {sliderType: 'saturation'},
                            },{
                                component: iro.ui.Slider,
                                options: {sliderType: 'value',},
                            },
                        ],
                    });
                    this.colorPicker.on("color:change", this.onUpdate.bind(this));
                    // console.log('componentDidMount popup picker');
                }
            }

            return this.wrapPopup(popup);
        }

        return block;
    }

}


class App_Color extends App_Common{
    addr_begin = 0x10;
    addr_end = 0x12;
    colorPicker;
    // constructor() {
    //     super();
    // }
    wsMessageCustom(e){
        let edata = e.data;
        if(edata.cmd == "eeprom"){
            let ee = edata.data;
            if(0x010 in ee && 0x011 in ee && 0x012 in ee){
                // console.log('ee',edata.data);
                if(this.colorPicker){
                    this.colorPicker.color.set({r: ee[0x010], g: ee[0x011], b: ee[0x012]});
                    // console.log(this.colorPicker);
                }
            }
        }
    }
    render(){
        let ee = this.state.EEPROM;
        let ul = <ul className="qlocktwo__letters">
            <li className="pop_header text-right">
                <a href="/"><span className="icon-house"></span></a>
            </li>
            <li className="i-split">Цвет</li>
            <li>&nbsp;</li>
            <li>
                <div id="picker" className="d-flex  justify-content-center"></div>
            </li>
            <li>&nbsp;</li>
            <li>
                CRGB( {ee[0x010]} , {ee[0x011]} , {ee[0x012]} );
            </li>
            {(!!this.state.hsv)?<li>CHSV( {Math.floor(this.state.hsv.h * 255 / 360)} , {Math.floor(this.state.hsv.s * 255 / 100)} , {Math.floor(this.state.hsv.v * 255 / 100)} );</li>:""}
            <li>&nbsp;</li>
            {(this.state.commited == false)?<li><a className="i-split" onClick={this.sendAction.bind(this,"commit",false)}>Запомнить</a></li>:""}
            {/**/}
        </ul>
        return this.wrapChildren(ul);
    }
    colorChangeCallback(e){
        document.body.style.backgroundColor = e.hexString;
        sendWS({cmd:"setColor",value:e.rgb});
        let ee = this.state.EEPROM;
            ee[0x10] = e.red;
            ee[0x11] = e.green;
            ee[0x12] = e.blue;
        this.setState({EEPROM:ee});
        this.setState({hsv:e.hsv});
    }
    componentDidMount(){
        let ee = this.state.EEPROM;
        // console.log({r: ee[0x010], g: ee[0x011], b: ee[0x012]});
        this.colorPicker = new iro.ColorPicker('#picker',{
            // color: {r: ee[0x010], g: ee[0x011], b: ee[0x012]},
            width: 270,
            layout:[
                {
                    component: iro.ui.Wheel,
                },{
                    component: iro.ui.Slider,
                    options: {sliderType: 'red',},
                },{
                    component: iro.ui.Slider,
                    options: {sliderType: 'green',},
                },{
                    component: iro.ui.Slider,
                    options: {sliderType: 'blue',},
                },
                {
                    component: iro.ui.Box,
                },
                {
                    component: iro.ui.Slider,
                    options: {sliderType: 'hue'},
                },
                {
                    component: iro.ui.Slider,
                    options: {sliderType: 'saturation'},
                },{
                    component: iro.ui.Slider,
                    options: {sliderType: 'value',},
                },
            ],
        });
        this.colorPicker.on("color:change", this.colorChangeCallback.bind(this));
    }
}

ReactDOM.render(<App_Colors/>, document.getElementById("app"));