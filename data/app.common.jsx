var sendWS = function(){}
var getNow = function(){}


class Component_Common extends React.Component{
    frameClasses = "qlocktwo qlocktwo--touch qlocktwo--text qlocktwo--reflection__ qlocktwo--shadow qlocktwo--fill-screen qlocktwo--lang-RU";
    // wrapSpan
    wrapChildren(node){
        if(node == null) return;
        // console.log(node,typeof node)
        if(node.className !== undefined){
            if(node.className.split(' ').indexOf('i-split') > -1){
                if(typeof node.children == "string"){
                    // console.log(node.className);
                    var el_txt = [];
                    for(var i in node.children){
                        el_txt.push(<span>{node.children[i]}</span>);
                    }
                    node.children = el_txt;
                    return node;
                }
            }
        }

        if(node.props !== undefined){
            if(node.props.children){
                if(typeof node.props.children == "string" && node.props.className !== undefined && node.props.className.split(' ').indexOf('i-split') > -1){
                    var el_txt = [];
                    for(var i in node.props.children){el_txt.push(<span>{node.props.children[i]}</span>);}
                    node.props.children = el_txt;
                    return node;
                }
                for(var i in node.props.children){
                    var el = node.props.children[i];
                    if(typeof el == "string" && node.props.className !== undefined && node.props.className.split(' ').indexOf('i-split') > -1){
                        var el_txt = [];
                        for(var j in el){el_txt.push(<span>{el[j]}</span>);}
                        node.props.children[i] = el_txt;
                        continue;
                    }
                    if(typeof el == "object"){
                        node.props.children[i] = this.wrapChildren(el);
                    }
                    //
                }
            }
        }
        return node;
    }
    wrapPopup(node){
        return <div className="pop">
            <div className="container">
                <div className={this.frameClasses+" text-light"}>
                    {node}
                </div>
            </div>
        </div>
    }
}
class App_Common extends Component_Common{

    host = location.hostname;
    ws;//
    cols=11;
    rows=10;
    SStimeInterval;
    SStimeOut;
    ticker500 = false;

    wsConnect(){
        let that = this;
        this.ws = new WebSocket("ws://"+this.host+":81");

        this.ws.onopen = function(){
            console.log("wsClient connected");
            that.wsMessage({data:{cmd:"connected"}});
            that.onConnect();
            //console.log('this.ticker500',that.ticker500);

            if(that.ticker500){
                that.SStimeInterval = setInterval(that.getNow.bind(that),500);
            }else{
                that.getNow();
            }

            //bc.postMessage({cmd:"connected"});
        }

        this.ws.onclose = function(){
            console.log("wsClient closed");
            that.wsMessage({data:{cmd:"closed"}});
            //bc.postMessage({cmd:"closed"});
            setTimeout(that.wsConnect.bind(that), 5000);
            clearInterval(that.SStimeInterval);
        }

        this.ws.addEventListener('message',this.wsMessageHandler.bind(this));
        // this.ws.onmessage = function(e){
        //     var data = JSON.parse(e.data);
        //
        //     this.wsMessage(data);
        // };
    }
    wsMessageHandler(e) {
        var data = JSON.parse(e.data);
        // console.log(data);
        this.wsMessage({data:data});
    }
    sendWS(data){
        this.ws.send(JSON.stringify(data));
        // console.log(this.ws)
        if(data.cmd == "setEEPROM"){
            var eedata = this.state.EEPROM;
                eedata[data.addr] = data.value;
            this.setState({EEPROM: eedata, commited: false });
        }
    }

    sendAction(cmd, value){
        this.sendWS({cmd:cmd,value:value});
        if(cmd == "commit" && this.ticker500 == false ) this.getNow();
    }
    getNow(){
        this.sendWS({cmd:"getNOW"});
    }

    addr_begin = 0;
    addr_end = 0;
    state_init = {};

    getDefaultState(){ return {}; }
    onConnect(){ return; }

    constructor(props) {
        super(props);
        sendWS = this.sendWS.bind(this);
        getNow = this.getNow.bind(this);


        this.state = Object.assign({
            EEPROM: {},
            commited: false,
        }, this.getDefaultState());

        if(this.addr_begin < this.addr_end){
            for(var i=this.addr_begin; i<this.addr_end; i++) this.state.EEPROM[i] = 0;
            this.sendWS({cmd:"getEEPROM",from:this.addr_begin,to:this.addr_end});
        }

        this.wsConnect();

    }

    wsMessage(e){
        let edata = e.data; // данные события

        if(edata.cmd == 'now'){
            edata.EEPROM = Object.assign(this.state.EEPROM,edata.EEPROM);
            // if (this.isComponentMounted)
                this.setState(edata);
        }
        if(edata.cmd == "connected"){
            console.log("connected");
            document.body.classList.toggle('loading',false);
            document.body.classList.toggle('connected',true);
            if(this.addr_begin < this.addr_end)
                sendWS({cmd:"getEEPROM",from:this.addr_begin,to:this.addr_end});
        }
        if(edata.cmd == "closed"){
            document.body.classList.toggle('loading',true);
            document.body.classList.toggle('connected',false);
            console.log("disconnected");
        }
        if(edata.cmd == "eeprom"){
            //console.log(this.state.EEPROM);
            //console.log(edata.data);
            let eedata = edata.data; // данные из объекта
            eedata[1024] = 0;
            if(e.type === 'message') eedata[1024] = 2;
            if(edata.local === true) eedata[1024] = 1;
            eedata = Object.assign(this.state.EEPROM,eedata);
            // if (this.isComponentMounted)
                this.setState({EEPROM: eedata});
        }
        if(this.wsMessageCustom !== undefined) this.wsMessageCustom(e);
    }

    fsList(data){
        // we need colors_slot_N.json
        var presets = [];
        for(var i in data){
            var file = data[i];
            if(file.type != 'file') continue;
            // colors_config_0.json
            if(file.name.match(/^colors_config_(\d+).json$/i)) presets.push(file.name);
        }
        // search 0 to 9 in presets
        let preset_list = [];
        for(var i=0; i<10; i++){
            var search_filename = "colors_config_"+i+".json";
            preset_list.push({
                n: i,
                name: search_filename,
                empty: (presets.indexOf(search_filename) < 0),
            });

        }
        this.setState({presets: preset_list});
        //console.log('fsList',data);
    }

    componentDidMount(){
        // var $this = $(ReactDOM.findDOMNode(this));
        // $this.find('[data-loc]').each(function(i,el){
        //     if('locale' in window){
        //         var loc_key = el.dataset.loc;
        //         if(loc_key in locale) el.innerText = locale[loc_key];
        //     }
        //     // console.log(el,i)
        // });
        // console.log('do split');
        // var that = ReactDOM.findDOMNode(this);
        // var splitNodes = that.querySelectorAll('.i-split');
        // splitNodes.forEach(function(el,i){
        //     var text = el.innerText.toString();
        //     console.log();
        //     var text_el = [];
        //     for (var str of text) {
        //         text_el.push("<span>"+str+"</span>");
        //     }
        //     // el.innerHTML = text_el.join('');
        // });
        // // return true;
    }
    // set el height and width etc.

    render(){
        return <ul className="qlocktwo__letters text-left">
            <li className="pop_header text-right">
                <a href="/"><span className="icon-house"></span></a>
            </li>
        </ul>;
    }

    componentDidUpdate(){

        // var $this = $(ReactDOM.findDOMNode(this));
        // console.log(ReactDOM.findDOMNode(this));
        // $this.find('.i-split').each(function() {
        //     var $that = $(this);
        //     var text = $that.text();
        //     console.log(text)
        //     var text_el = [];
        //     for (var i in text) {
        //         text_el.push($('<span>', {text: text[i]}));
        //     }
            // $that.html(text_el);
        // });
    }
}

class InputRange extends React.Component{
    onUpdate(value){}
    getDefaultState(props){
        return {
            value : (!!props.value) ? props.value : 0,
        };
    }
    componentWillReceiveProps(nextProps){
        this.setState(this.getDefaultState(nextProps));
    }
    eventChange(e){
        var v = e.target.value;
        this.setState({value:v});
        if(!!this.props.addr) sendWS({cmd:"setEEPROM",addr:this.props.addr,value:v});
        if(!!this.props.onUpdateCmd) sendWS({cmd:this.props.onUpdateCmd});
        this.onUpdate(e);
    }
    constructor(props) {
        super(props);
        console.log('constructor',props);
        this.state = this.getDefaultState(props);
        if(!!props.onUpdate) this.onUpdate = props.onUpdate;
        // if(!!props.addr) this.onUpdate = props.onUpdate;
    }

    render(){
        let min = this.props.min || 0;
        let max = this.props.max || 255;
        return <input className="i-range" type="range" min={min} max={max} value={this.state.value} onChange={this.eventChange.bind(this)} />
    }
}


class Flags extends React.Component {

    getDefaultState(props){
        let valBin = parseInt(props.value).toString(2);
        valBin = "0".repeat(8 - valBin.length) + valBin;
        return {
            value: props.value,
            values: valBin
        };
    }

    componentWillReceiveProps(nextProps){
        this.setState(this.getDefaultState(nextProps));
    }

    eventChangeHandler(i, e) {
        const oldVal = parseInt(this.state.value);
        let newVal = this.updateBit(oldVal, i, e.target.checked);
        let newValBin = parseInt(newVal).toString(2);
        newValBin = "0".repeat(8 - newValBin.length) + newValBin;
        // this.setState({
        //     value: newVal,
        //     values: newValBin,
        // });
        if(!!this.props.addr) sendWS({cmd:"setEEPROM",addr:this.props.addr,value:newVal});
        if(!!this.props.onUpdateCmd) sendWS({cmd:this.props.onUpdateCmd});
    }
    /*
  https://askdev.ru/q/kak-ustanovit-ochistit-i-pereklyuchit-odin-bit-v-javascript-62382/
  */
    updateBit(number, bitPosition, bitValue) {
        const bitValueNormalized = bitValue ? 1 : 0;
        const clearMask = ~(1 << bitPosition);
        return (number & clearMask) | (bitValueNormalized << bitPosition);
    }

    render() {
        var labels = [];
        if(!this.state) return null;
        var values = this.state.values; //parseInt(this.props.value).toString(2);
        // console.log(values);
        for (var i in this.props.keys) {
            let elementID = (!!this.props.addr) ? 'sw_'+this.props.addr.toString(16)+'_'+i : 'sw'+i;
            let act = (values[7-i] == "1");
            let icon = <i className={(act)?'icon-check_circle_outline':'icon-check_box_outline_blank'}></i>
            labels.push(
                <div className={(act) ? '':'disabled'}>
                    <input
                        id={elementID}
                        type="checkbox"
                        className="d-none"
                        checked={act}
                        onChange={this.eventChangeHandler.bind(this, i)}
                    />
                    <label for={elementID}>{icon} {this.props.keys[i]}</label>
                </div>
            );
        }
        return (
            <div>
                {labels}
                <span className="d-none">value: {this.state.value}</span>
            </div>
        );
    }
}

class RangePeaker extends Component_Common{
    constructor(props) {
        super(props);
        this.state ={
            popup: false
        }
    }
    togglePopup(){
        this.state.popup = !this.state.popup;
        this.setState(this.state);
    }

    eventChange(v){
        var f = this.props.field;
        if(this.props.cb) this.props.cb(f,v);
        this.togglePopup();
    }
    render(){

        let ce = (this.props.eventChange) ? this.props.eventChange.bind(this) : this.eventChange;

        var list = [];
        for(let i = this.props.min; i<=this.props.max; i++){
            let classList = ["text-nowrap"];
            if(i == this.props.value) classList.push("text-success");
            let el = <a key={i} className={classList.join(" ")} onClick={this.eventChange.bind(this,i)}><span>{('0' + i).slice(-String(this.props.max).length)}</span></a>;
            list.push( (!!this.props.w) ? this.wrapChildren(el) : el);
            // list.push(this.wrapChildren(el));
        }
        var className = "i-split";
        if(this.state.popup) className+=" text-danger";
        var element = <a className={className} onClick={this.togglePopup.bind(this)}>{('0' + this.props.value).slice(-String(this.props.max).length)}</a>
        if(this.state.popup){
            var popup = <ul className="qlocktwo__letters qlocktwo__letters__7 row">
                <li className="pop_header text-right"><a onClick={this.togglePopup.bind(this)}>x</a></li>{list}
            </ul>
            return this.wrapPopup(popup);
        }
        return (!!this.props.w) ? this.wrapChildren(element) : element;
        // return element;
        // return this.wrapChildren(element);
    }
}

class ActionButton extends React.Component{
    /*
    * cmd:
    * label:
    * subclass: [primary, secondary, success, danger, warning, info, light, dark, link]
    * // https://getbootstrap.com/docs/4.0/components/buttons/
    * */
    eventClick(e){
        sendWS({cmd:this.props.cmd,value: this.props.value});
    }

    render(){
        let className="btn";
        if(this.props.subclass !== undefined){
            if(typeof this.props.subclass == "string") className+=" btn-"+this.props.subclass;
        }
        var but_label = this.props.label;
        if(this.props['data-loc']){
            var loc_key = this.props['data-loc'];
            if('locale' in window) if(loc_key in locale){
                // console.log(this.props);
                if(this.props.label !== undefined)
                    but_label = locale[loc_key];
            }
        }
        // console.log(this.props) className={className}
        return <a onClick={this.eventClick.bind(this)}>{but_label}</a>
    }
}