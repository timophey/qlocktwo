class App_Test extends App_Common {
    render(){
        return <ul className="qlocktwo__letters text-left">
            <li><a href="test.matrix.html">Pixels</a></li>
            <li>Mode:</li>
            <li><ActionButton type="button" subclass="primary" cmd="setDisplayMode" value="0" label="Time"></ActionButton></li>
            <li><ActionButton type="button" subclass="primary" cmd="setDisplayMode" value="2" label="ColorPallete"></ActionButton></li>
        </ul>
    }
}

ReactDOM.render(<App_Test/>, document.getElementById("app"));
