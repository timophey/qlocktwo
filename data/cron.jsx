class App_Cron extends App_Common{

    filename = ".crontab.json";

    defaultItem = {
        event: "time",
        time: {i:'*',h:'*',d:'*',m:'*',w:'*',},
        cmd: 'reloadColors',
        args: [],
    }
    getDefaultState(){return{
        items:[],
        saved: true,
    }}
    fetchData(){
        let that = this;
        fetch('/'+this.filename)
            .then((response) => {
                return response.json();
            },(response) => {
                console.log('fail',response);
            }).then(that.onFetchFile.bind(that));
    }
    onFetchFile(config){
        if(typeof config == 'object')
            this.setState({items:config.items})
    }
    componentDidMount(){
        this.fetchData();
    }
    handlerAddItem(){
        this.state.items.push(this.defaultItem);
        this.state.saved = false;
        this.setState(this.state);
    }
    handlerUpdateItem(i,e){
        if(e) this.state.items[i]=e;
        if(e==null) delete this.state.items[i];
        this.state.saved = false;
        this.setState(this.state);
    }
    handlerSave(){
        let formData = new FormData();
        let itemList = this.state.items.filter((item)=>{return !!item;});
        let that = this;
        var json = JSON.stringify({items: itemList});
        formData.append('data', new File([new Blob([json])], this.filename));
        fetch('/upload', {
            method: 'POST',
            body: formData
        }).then(()=>{
            that.state.saved = that;
            that.setState(that.state);
        });
    }
    render(){
        let that = this;
        return <ul className="qlocktwo__letters text-left">
            <li className="pop_header text-right">
                <a href="/"><span className="icon-house"></span></a>
            </li>
            {(this.state.items)?this.state.items.map(function(item,i){
                return <App_CronItem item={item} index={i} key={i} onUpdate={that.handlerUpdateItem.bind(that,i)} />
            }):<li>Emply List</li>}
            <li className="text-right">
                <a onClick={this.handlerAddItem.bind(this)}>[+]</a>
            </li>
            <li className={(this.state.saved)?'d-none':'d-block'}>
                <a onClick={this.handlerSave.bind(this)}>Save</a>
            </li>
        </ul>;
    }
}

class App_CronItem extends React.Component{
    state = {
        item:{},
        edit:false
    };
    events = [
        'time',
        'boot',
        'connect',
    ];
    commands = [
        {t:'reloadColors',agrs:0},
        {t:'setConfig',args:1},
        {t:'updatentp',args:0,label:'Update by NTP'},
    ]
    componentDidMount(){
        this.setState({item:this.props.item});
    }
    componentDidUpdate(prevProps, prevState, snapshot){
        // if(this.props.index != prevProps.index)
        // this.setState({item:this.props.item});
    }
    toggleEditState(){
        if(this.state.edit == true && !!this.props.onUpdate) this.props.onUpdate(this.state.item);
        this.setState({edit:!this.state.edit});
    }
    doFire(){
        if(!!this.props.onUpdate) this.props.onUpdate(null);
    }
    handleSetItemTime(f,e){
        console.log(e.target.value,f)
        this.state.item.time[f] = e.target.value;
        this.setState(this.state);
    }
    render(){
        let item = this.state.item;
        let that = this;
        // обработчик правки аргумента
        let argfh = (i,e)=>{
            this.state.item.args[i] = e.target.value;
            this.setState(this.state);
        }
        // показывает аргументы
        let argf = (ac)=>{
            var args=[];
            for(var i=0; i<ac; i++){
                let val = (!!item.args[i])? item.args[i] : 0;
                args.push(<div key={i}>arg{i}:<input value={val} onChange={argfh.bind(that,i)}/></div>);
            }
            return <div>{args}</div>
        };
        // текущая команда
        let _cmd = this.commands.filter(function (cmd) {
            console.log(item);
            return (!!item) ? (cmd.t == item.cmd) : false;
        });
        let cmd = (_cmd)? _cmd[0] : this.commands[0];
        console.log(cmd);
        // рендер
        return (this.state.edit)?<li>
            <div className="clearfix">
                <a onClick={this.toggleEditState.bind(this)} className="float-right"><i className="icon-pencil"></i></a>
            </div>
            <div className="text-nowrap">
                on <select onChange={(e,a)=>{ that.state.item.event = e.target.value; that.setState(that.state); }} value={item.event}>{this.events.map((e,i)=>{ return <option key={i}>{e}</option> })}</select>
            </div>
            <div className={(item.event=='time')?'d-block':'d-none'}>
                time:
                <input className="ci0" value={item.time.i} onChange={this.handleSetItemTime.bind(this,'i')} placeholder="i"/>:
                <input className="ci0" value={item.time.h} onChange={this.handleSetItemTime.bind(this,'h')} placeholder="h"/>,
                <input className="ci0" value={item.time.d} onChange={this.handleSetItemTime.bind(this,'d')} placeholder="d"/>/
                <input className="ci0" value={item.time.m} onChange={this.handleSetItemTime.bind(this,'m')} placeholder="m"/>,
                <input className="ci0" value={item.time.w} onChange={this.handleSetItemTime.bind(this,'w')} placeholder="w"/>
            </div>
            <div className="text-nowrap">
                cmd <select onChange={(e,a)=>{ that.state.item.cmd = e.target.value; that.setState(that.state); }} value={item.cmd}>{this.commands.map((e,i)=>{ return <option key={i} value={e.t}>{((e.label)?e.label:e.t)}</option> })}</select>
            </div>
            {argf(cmd.args)}
            <hr/>
        </li>:<li className="clearfix">
            #{this.props.index}:&nbsp;
            {(!!item)?item.cmd:'?'}
            {(!!item.args)?'('+item.args.join(',')+')':'()'}
            <a onClick={this.doFire.bind(this)} className="float-right"><span className="icon-remove"></span></a>
            <a onClick={this.toggleEditState.bind(this)} className="float-right"><i className="icon-pencil"></i></a>
        </li>
    }
}
