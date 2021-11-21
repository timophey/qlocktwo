class App_Wifi extends App_Common {

    getDefaultState(){return{
        networks:[],
        ifconfig:{
            RSSI: 0, SSID: "", dnsIP1: "", dnsIP2: "", gatewayIP: "", hostname: "", localIP: "", softAPIP: "", status: 6, subnetMask: "",
        },
        connections:[],
        process: false
    }}

    loadConnections(){
        $.getJSON('http://'+this.host+"/wifi_connections.json",this.loadConnectionsCallback.bind(this));
    }
    loadConnectionsCallback(data){
        console.log('loadConnectionsCallback',data);
        this.setState({connections: data});
    }

    actionScan(){
        document.body.classList.toggle('loading',true);
        sendWS({cmd:"iwconfig",args:['scan']});
        this.loadConnections();
        this.setState({progress:true});
        return false;
    }

    onConnect(){
        // this.actionScan();
    }

    wsMessageCustom(e){
        this.setState({progress:false});
        let edata = e.data;
        if(edata.cmd == "connected"){
            this.actionScan();
        }
        if(edata.cmd =="iwconfig"){
            document.body.classList.toggle('loading',false);
            if(edata.network !== undefined) this.setState(Object.assign(this.state,{networks:edata.network}));
            if(edata.ifconfig !== undefined) this.setState(Object.assign(this.state,{ifconfig:edata.ifconfig}));
        }
    }


    constructor(props) {
        super(props);
        this.loadConnections();
    }

    render(){
        let that = this;
        let networks = this.state.networks;
        // перебираем все соединения из списка сохранённых
        for(var connection of this.state.connections){
            let found = false;
            for(var network of networks){
                if(connection.SSID == network.SSID){ found = true; break;}
            }
            if(!found) networks.push({SSID:connection.SSID,RSSI:-100});
        }



        // let items =
        return <ul className="qlocktwo__letters text-left">
            <li className="pop_header text-right">
                <a href="/"><span className="icon-house"></span></a>
            </li>
            {networks.map((item,i)=>{
            let conf = {};
            if(that.state.connections){
                //console.log(that.state.connections)
                for(var connection of that.state.connections)
                    if(connection.SSID == item.SSID) conf = connection;
            }

            return <Connection
                key={i} ssid={item.SSID} rssi={item.RSSI} state={that.state} conf={conf}
                actionScanHandler={that.actionScan.bind(that)}
                loadConnectionsHandler={that.loadConnections.bind(that)}
            />
        })}</ul>
    }



}

class Connection extends Component_Common{

    getDefaultState(props){
        return {
            opened: false, ssid:props.ssid, password: "", password_hidden: true, progress: false, popup: false
        };
    }

    constructor(props) {
        super(props);
        this.state = this.getDefaultState(props);
    }

    togglePopup(){
        this.state.popup = !this.state.popup;
        this.setState(this.state);
    }
    isConnected(){return (this.props.state.ifconfig.SSID == this.props.ssid && this.props.state.ifconfig.status == 3);}
    handleClick(){this.setState(Object.assign(this.state,{opened: !this.state.opened }));}
    handleInputPassword(e){this.setState(Object.assign(this.state,{password: e.target.value }));}
    handleShowPassword(e){this.setState(Object.assign(this.state,{password_hidden: !this.state.password_hidden }));}

    handleConnect(){
        var pass = (!!this.props.conf.PASS) ? this.props.conf.PASS : this.state.password;
        console.log('handleConnect',this.props.ssid,pass);
        sendWS({cmd:"iwconfig",args:['connect',this.props.ssid,pass]});
        this.handleClick();
        this.setState({progress:true});
        console.log("Connect to "+this.props.ssid+":"+pass);
        return false;
    }
    handleDisconnect(){
        sendWS({cmd:"iwconfig",args:['disconnect']});
        this.props.actionScanHandler();
        console.log("Disconnect");
        return false;
    }
    handleRemove(){
        if(!confirm("Remove "+this.props.ssid+" ?")) return false;
        sendWS({cmd:"iwconfig",args:['remove',this.props.ssid]});
        this.handleDisconnect();
        this.props.actionScanHandler();
        this.props.loadConnectionsHandler();
        // console.log("Disconnect");
        return false;
    }
    componentWillReceiveProps(nextProps){
        if(this.state.ssid != nextProps.ssid) this.setState(this.getDefaultState(nextProps));
    }

    render(){
        let classList = ["row","justify-content-between","align-items-center","alert"];
        let isConnected = this.isConnected();
        let isConfigured = !!this.props.conf.PASS;
        if(isConnected && this.state.progress) this.setState({progress:false});
        if(this.props.rssi > -100) classList.push((isConfigured)?((isConnected)?'alert-success':'alert-secondary'):'alert-secondary'); else classList.push('alert-secondary');
        let iconClassName = "icon-connection" + ((this.state.progress) ? " gly-beat" : (isConnected)?" text-success":"");
        let element = <li data-popup={this.state.popup}>
            <a className="d-block" onClick={this.togglePopup.bind(this)}>
                <i className={iconClassName}
                   style={{opacity: (1 + this.props.rssi / 100)+.5}} title={"RSSI: "+this.props.rssi}>&nbsp;</i>
                {this.props.ssid} {/*({this.props.rssi})*/} {(isConfigured) ?
                <i className="icon-star-full text-warning"></i> : ""} &nbsp;
            </a>
        </li>;
        if(this.state.popup){
            var popup = <ul className="qlocktwo__letters text-left">
                <li className="pop_header text-right"><a onClick={this.togglePopup.bind(this)}>x</a></li>
                <li><i className={iconClassName}></i> {this.props.ssid} {(isConfigured) ? <i className="icon-star-full text-warning"></i> : ""}</li>
                {(isConnected)?
                    <div>
                        <li className="text-left">IP: {this.props.state.ifconfig.localIP}</li>
                        <li className="text-left">Host: {this.props.state.ifconfig.hostname}</li>
                        <li>
                            <a onClick={this.handleDisconnect.bind(this)}><i className="icon-flash_off text-danger"></i> Отключиться</a>
                        </li>
                        <li>
                            <a onClick={this.handleRemove.bind(this)}><i className="icon-remove text-danger"></i> Забыть</a>
                        </li>
                    </div>:<div>
                    <li className="">
                        <span><i className="icon-key">&nbsp;</i></span>
                        <input onChange={this.handleInputPassword.bind(this)} type={(this.state.password_hidden)?"password":"text"}/>
                    </li>
                    <li>
                        <label>
                            <button type="button" onClick={this.handleShowPassword.bind(this)}><i className={(this.state.password_hidden)?'icon-eye':'icon-eye-blocked'}></i></button>
                            {(this.state.password_hidden)?'показать':'скрыть'} пароль
                        </label>
                    </li>
                    <li>
                        <label>
                            <button type="button" onClick={this.handleConnect.bind(this)}><i className="icon-flash_on"></i></button>
                            Подключиться
                        </label>
                    </li>

                </div>}
            </ul>
            return this.wrapPopup(popup);
        }
        return element;
    }
}

ReactDOM.render(<App_Wifi/>, document.getElementById("app"));