class App_Geometry extends App_Common{

    addr_begin = 0x00;
    addr_end = 0x03;

    getDefaultState(){ return {ifconfig:{}}; }

    // constructor() {
    //     super();
    //     let ifconfig_promise = await fetch("/ifconfig.json");
    //     if (ifconfig_promise.ok){
    //         let ifconfig_json = await ifconfig_promise.json();
    //         let ifconfig = JSON.parse(ifconfig_json);
    //         console.log('ifconfig',ifconfig);
    //     }
    // }
    componentDidMount() {
        fetch("/ifconfig.json")
            .then(response => response.json())
            .then(result => this.setState({ifconfig: result}));
    }
    render(){
        let ee = this.state.EEPROM;

        let byte_wifi_keys = {
            0: 'Enable FTP',
            1: 'Enable OTA',
            2: 'Show IP on boot',
            4: 'softAP opened',
            5: 'softAP hidden',
            6: 'softAP keep up',

        }
        let byte_webs_keys = {
            // 0: 'xx',
            2: 'Enable Cache for HTTP',
            3: 'Enable CaptivePortal',
            4: 'Allow EDIT actions',
            5: 'Allow WEB Upgrade',
            7: 'Stand DEMO mode',
        }
        return <ul className="qlocktwo__letters text-left">
            <li className="pop_header text-right">
                <a href="/"><span className="icon-house"></span></a>
            </li>
            <li>WiFi config</li>
            <li>
                <Flags keys={byte_wifi_keys} value={ee[0x01]} addr={0x01}/>
            </li>
            <li>&nbsp;</li>
            <li>Webserver config</li>
            <li>
                <Flags keys={byte_webs_keys} value={ee[0x02]} addr={0x02}/>
            </li>
            <li>&nbsp;</li>
            <li>soft AP QR Code</li>
            {(this.state.ifconfig.softAPSSID !== undefined) ? <li>
                <App_WiFi_QR SSID={this.state.ifconfig.softAPSSID} PSK={this.state.ifconfig.softAPPSK} />
            </li>:""}
            <li>&nbsp;</li>
            <li>soft AP Access</li>
            {(this.state.ifconfig.softAPIP !== undefined) ? <li>
                <QRCode href={"http://"+this.state.ifconfig.softAPIP} />
            </li>:""}
            <li>&nbsp;</li>
            {
                (this.state.commited == false)?
                    <li><a className="i-split" onClick={this.sendAction.bind(this,"commit",false)}>Commit</a></li>
                :
                    <li className="text-right"><a className="i-split" onClick={this.sendAction.bind(this,"reboot",false)}>Reboot</a></li>
            }
        </ul>
    }

}

class App_WiFi_QR extends  React.Component{
    generate () {
        var $this = $(ReactDOM.findDOMNode(this));
        var ssid = this.props.SSID;//"Qlocktwo-e52b6a";//$('#ssid').val();
        if(!ssid) return;
        var hidden = $("#sw_1_5").is(':checked');
        var enc = $('#sw_1_4').is(':checked') ? 'nopass':'WPA';
        if (enc != 'nopass') {
            var key = this.props.PSK;//"12345678";//$('#key').val();
            $('#showkey').text(enc+' Passphrase: '+key);
        } else {
            var key = '';
            $('#showkey').text('');
        }
        // https://github.com/zxing/zxing/wiki/Barcode-Contents#wi-fi-network-config-android-ios-11
        var qrstring = 'WIFI:S:'+escape_string(ssid)+';T:'+enc+';P:'+escape_string(key)+';';
        if (hidden) {
            qrstring += 'H:true';
        }
        qrstring += ';';

        var $qrcode = $this.find('.qrcode');

        $qrcode.empty();
        $qrcode.qrcode(qrstring);
        // $('#showssid').text('SSID: '+ssid);
        // $('#save').show();
        // $('#print').css('display', 'inline-block');

        var canvas = $qrcode.find('canvas');
        if (canvas.length == 1) {
            var data = canvas[0].toDataURL('image/png');
            $qrcode.attr('href', data);
            $qrcode.attr('download', ssid+'-qrcode.png');
            // e.show() sets display:inline, but we need inline-block
            // e.css('display', 'inline-block');
        }
    };
    componentWillReceiveProps(nextProps){
        this.generate();
    }
    render(){
        return <div>
                <a id="qrcode_wifi" className="qrcode"></a>
            </div>;
    }
}

class QRCode  extends React.Component{
    generate () {
        console.log(this);
        if(!this.props.href) return;
        var $this = $(ReactDOM.findDOMNode(this));
        var $qrcode = $this.find('.qrcode');
        $qrcode.empty();
        $qrcode.qrcode(this.props.href);

        var canvas = $qrcode.find('canvas');
        if (canvas.length == 1) {
            var data = canvas[0].toDataURL('image/png');
            $qrcode.attr('href', data);
            $qrcode.attr('download', 'access-qrcode.png');
        }
    };
    componentWillReceiveProps(nextProps){
        this.generate();
    }
    render(){
        return <div>
            <a className="qrcode"></a>
        </div>;
    }
}

function escape_string (string) {
    var to_escape = ['\\', ';', ',', ':', '"'];
    var hex_only = /^[0-9a-f]+$/i;
    var output = "";
    for (var i=0; i<string.length; i++) {
        if($.inArray(string[i], to_escape) != -1) {
            output += '\\'+string[i];
        }
        else {
            output += string[i];
        }
    }
    //if (hex_only.test(output)) {
    //    output = '"'+output+'"';
    //}
    return output;
};
