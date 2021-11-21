class App_Presets extends App_Common {
    addr_begin = 216;
    addr_end = 217;

    getDefaultState(){return{
        files:[],
        current: null,
    }}
    componentDidMount(){
        this.fileListUpdate();
    }
    fileListUpdate(){
        let that = this;
        fetch('/list?dir=/&startsWith=.config_')
            .then((response) => {
                return response.json();
            }).then(that.onFetchFileList.bind(that));
    }
    onFetchFileList(list){
        // if (this.isComponentMounted)
            this.setState({files:list});
    }

    onSetCurrent(k,v){
        this.setState({current:v});
    }

    onSaveCurrent(){
        sendWS({cmd:"setConfig",value:this.state.current,write:1});
        this.fileListUpdate();
        // this.setState({current:v});
    }

    eventChange(k,v){
        this.state.now[k]=parseInt(v);
        this.setState(this.state);
        sendWS({cmd:"setTime",now:this.state.now});
    }

    render(){

        let element = <ul className="qlocktwo__letters text-left">
            <li className="pop_header text-right"><a href="/"><span className="icon-house"></span></a></li>
            <li className="i-split">Save & restore</li>
            {this.state.files.map((item,i)=>{
                let fn = item.name;
                let m = fn.match(/^.config_([0-9a-f]+)/i); if(!m) return null;
                let fi = parseInt(m[1],16);
                console.log(fi)
                return <App_Preset fn={item.name} key={i} current={0} index={fi} onRemove={this.fileListUpdate.bind(this)}/>
            })}
            <li>&nbsp;</li>
            {/*<li><InputRange value={0xFF}/></li>*/}
            {(!!this.state.EEPROM)?<li className="i-split">
                Save as:
                <a className="float-right" onClick={this.onSaveCurrent.bind(this)}>&nbsp; <span className="icon-flash_on"></span> </a>
                <span className="float-right"><RangePeaker field="curr" value={(this.state.current === null) ? this.state.EEPROM[216] : this.state.current} min={0} max={255} cb={this.onSetCurrent.bind(this)}/></span>
            </li>:""}
        </ul>;
        return element;
        return this.wrapChildren(element);
    }


}

class App_Preset extends React.Component{
    color;
    state = {
        ee:{}
    }
    //             onClick={this.togglePopup.bind(this)}
    render(){
        let c = 'black';
        let ee = this.state.ee;
        let valBin = '00000000';
        if(this.state.ee){
            valBin = parseInt(ee[0x03]).toString(2);
            valBin = "0".repeat(8 - valBin.length) + valBin;
            // c = (valBin[7-0]=='1') ? "hsl("+ee[13]/255*360+","+ee[14]/255*100+"%,"+ee[15]/255*100+"%)" : "rgb("+ee[16]+","+ee[17]+","+ee[18]+")";
            c = "rgb("+ee[16]+","+ee[17]+","+ee[18]+")"
            console.log(c);
        }
        console.log(valBin);
        let block = <div
            style={{display:"inline-block",backgroundColor:c,height:"1em",width:"1em",verticalAlign: "middle"}}
        ></div>;
        // let m = this.props.fn.match(/^.config_(\d+)/i); if(!m) return;
        // let fi = m[1];
        return <li className="clearfix">
            #{this.props.index}: {(valBin[7-0]=='1')?'HSV':'RGB'} ({(valBin[7-0]=='1' && valBin[7-7]=='1') ? 'random': block })
            <a className="float-right" onClick={this.doLoad.bind(this)}><span className="icon-lab"></span></a>
            <a className="float-right" onClick={this.doFire.bind(this)}><span className="icon-remove"></span></a>
            </li>
    }

    doLoad(){
        if(!!this.props.index)
            sendWS({cmd:"setConfig",value:this.props.index,write:0});
    }

    doFire(){
        let that = this;
        if(confirm("Delete?")){
            fetch('/edit?path=/'+this.props.fn,{method: 'DELETE'}).then((response) => {
                alert('Deleted');
                if(!!that.props.onRemove) that.props.onRemove();
            })
        }
        // fetch('/article/fetch/post/user',
    }
    fetchData(){
        let that = this;
        fetch('/'+this.props.fn)
            .then((response) => {
                return response.json();
            }).then(that.onFetchFile.bind(that));

    }
    componentDidUpdate(prevProps, prevState, snapshot){
        if(this.props.index != prevProps.index) this.fetchData();
    }
    componentDidMount(){
        this.fetchData();
    }

    onFetchFile(config){
        this.setState({ee:config.data})
    }
}