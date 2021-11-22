class App_Test_Matrix extends App_Common {
    getDefaultState() {
        var m=[];
        for(var j=0;j<this.cols;j++){ // matrix[x][y]
            m[j]=[]; for(var i=0;i<this.rows;i++) m[j][i] = undefined;
        }
        return {timersStarted:true,matrix:m};
    }
    toggleTimers(){
        this.state.timersStarted = !this.state.timersStarted;
        this.setState(this.state);
        this.sendAction("startDisplayTimers",this.state.timersStarted ? 1 : 0);
        // this.sendAction.bind(this,"startDisplayTimers",1)
    }
    lightsOff() {
        this.sendAction("lightsOff");
        for (var j = 0; j < this.cols; j++) for(var i = 0; i < this.rows; i++) this.state.matrix[j][i]=0;
        this.setState(this.state);
    }
    setPixel(x,y,value){
        if(x > this.cols || y > this.rows) return;
        this.state.matrix[x][y] = value;
        this.setState(this.state);
    }
    togglePixel(x,y,value){
        if(x > this.cols || y > this.rows) return;
        var newValue = (value === undefined) ? (this.state.matrix[x][y] ? 0 : 1 ) : value;
        this.state.matrix[x][y] = newValue;
        // this.sendAction("setPixel",);
        var pxVal = newValue ? 0xFFFFFF : 0;
        this.sendWS({cmd:"setPixel",x:x,y:y,value:pxVal});
        this.setState(this.state);
    }

    render(){
        let matrix = [];
        for(var i=0;i<this.rows;i++){
            var string = [];
            for(var j=0;j<this.cols;j++){  // matrix[x][y]
                var v = this.state.matrix[j][i];
                string.push(<span onClick={this.togglePixel.bind(this,j,i,undefined)}>{(v == undefined)?"?":v}</span>)
            }
            matrix.push(<li>{string}</li>);
        }
        return <ul className="qlocktwo__letters text-left">
            <li className="pop_header text-right">
                <a href="/"><span className="icon-house"></span></a>
            </li>
            <li className="text-left">
                <a onClick={this.toggleTimers.bind(this)}>{this.state.timersStarted ? "Stop":"Restart"} timers</a>
            </li>
            <li className="">
                <a onClick={this.lightsOff.bind(this)}>Lights OFF</a>
            </li>
            {matrix}
        </ul>
    }
}

// class Action

ReactDOM.render(<App_Test_Matrix/>, document.getElementById("app"));
