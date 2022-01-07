class App_Demo extends React.Component{

    letters = [
        ["О","Д","И","Н","П","Я","Т","Ь","Д","В","А"],
        ["Д","Е","Ш","Е","С","Т","Ь","В","Я","Т","Ь"],
        ["В","О","Ч","Е","С","Е","М","Ь","Т","Р","И"],
        ["Т","Ы","Д","В","Е","Р","Е","С","Я","Т","Ь"],
        ["Н","А","Д","Ц","А","Т","Ь","Ч","А","С","А"],
        ["Ч","А","С","О","В","Д","С","О","Р","О","К"],
        ["Т","Р","И","Д","В","А","Д","П","Я","Т","Ь"],
        ["П","Я","Т","Н","А","Д","Е","Ц","А","Т","Ь"],
        ["А","М","Д","Е","С","Я","Т","С","Я","Т","Ь"],
        ["П","Я","Т","Ь","М","И","Н","У","Т","Ы","А"]
    ];
    ss = false;
    st = false;
    print_buffer = [];
    shift_buffer = [];
    _switchDelay = 5000;
    _pd = 70;
    // 

    constructor(props){
        super(props);
        this.state = {
            leds: [], // 110 LEDS
        }
        this.scheduledHourMin();
        setInterval(this.scheduledHourMin.bind(this),this._switchDelay);
        setInterval(this._type.bind(this),this._pd);
    }


    scheduledHourMin(){
        let now = new Date();
        let h = now.getHours()
        let m = now.getMinutes();
        let value = (this.ss) ? m : h;
        this.showTimeWords(value,(this.ss)?2:1);
        this.ss = !this.ss;
    }

    showTimeWords(d,unit_t){
        
        let is_min = (unit_t > 1);
        let u00 = this.u00, u10 = this.u10, y00 = this.y00, y10 = this.y10, up = [], hl = this.hl, ml = this.ml;
        
        this.lightsDown();

        // split value
        let d10 = Math.floor(d / 10); // десятки
        let  d0 = d % 10;  // единицы
        
        /* Десятки
        * */
        if(d > 20){
            
            if(is_min && (d % 10 == 1) && (y10[d10][0] > 0)){ // x1
                this.lightsUp(y10[d10],10);
            }else{
                if(is_min && (d10 == 2) && (d0 == 3 || d0 == 5 || d0 == 6  || d0 == 7)){ // 23,25,26,27 min
                    this.lightsUp(u00[20],8);
                }else
                this.lightsUp(u10[d10],10);
            }
            if(d % 10) d = d % 10;
        }


        /* Единицы
        * */
        
        if(d <= 20){
            d0 = d;// % 10;
            if(is_min && 
                (
                (d > 0) && (d < 3) 
                || (d10 == 5) && (d0==5) && st /** 55 */
                || (d10 == 2 || d10 == 4) && (d0==3) /** 2x 4x && x3 */
                )
            ){
                this.lightsUp(y00[d0],4); // одна/две/ 55 / 43
            }else{
                if(is_min && d0 == 15 ){ // 15 min
                    up = [2,17,22,37,42,57,77,82,97,102];
                    this.lightsUp(up,10);
                }else
                if(is_min && (d10 == 2 || d10 == 4) && (d0 == 5)){ // 25,45 min
                    up = [76,83,96,103];
                    this.lightsUp(up,4);
                }else{
                    this.lightsUp(u00[d0],12);
                }
            }
        }        

        /*
        Часов, час, часа
        */
        if(unit_t == 2){
            //    Serial.println("min");
                if(d==1) this.lightsUp(ml[1],6); 
                else if(d>=2 && d<=4) this.lightsUp(ml[2],6);
                else this.lightsUp(ml[0],5);
            }
            if(unit_t == 1){
            //    Serial.println("hour");
                if(d==1){
                let pos = this._HLiterator % 2;
                this.lightsUp(hl[pos],3);              // x1
                }
                else if(d>=2 && d<=4) this.lightsUp(hl[2],4); // x2,x3,x4
                else this.lightsUp(hl[0],5);                  // ...
                this._HLiterator++;
            }

        // console.log(d,unit_t,d10,d0);
        let td = (this._switchDelay) - ((this.print_buffer.length+1) * 1 * this._pd);
        setTimeout(this._tail.bind(this),td);
    }

    lightsDown(){
        this.setState({leds:[]});
        this.print_buffer = [];
        this.shift_buffer = [];
    }

    lightsUp(ls, len){
        let leds = this.state.leds;
        for(let i = 0; i<len; i++){
            let l = ls[i];
            // leds[l] = true;
            if(l===undefined) continue;
            if(l==255 || l==0 && i>0) continue;
            this.print_buffer.push(l);
        }
        // this.setState({leds:leds});
    }
    
    _type(){
        if(this.print_buffer.length == 0) return;
        let leds = this.state.leds;
        let l = this.print_buffer.shift();
        leds[l] = true;
        this.shift_buffer.push(l);
        this.setState({leds:leds});
    }

    _tail(){
        let leds = this.state.leds;
        let l = this.shift_buffer.shift();
        leds[l] = false;
        this.setState({leds:leds});
        if(this.shift_buffer.length > 0){
            setTimeout(this._tail.bind(this),this._pd);
        }
        
    }


    render(){
        let lines = [];
        let lsrc = this.letters;
        for(let i in lsrc){
            let strings = [];
            for(let j in lsrc[i]){
                let char  = lsrc[i][j];
                let L = 9 - i + 10 * j;
                if(Math.floor(L / 10) % 2 > 0) L = (Math.floor(L / 10)+1)*10 - (L%10 + 1);
                let cl = (!!this.state.leds[L])?'light':'';
                strings.push(<span className={cl}>{char}</span>);
            }
            lines.push(<li>{strings}</li>)
        }
        return <ul className="qlocktwo__letters text-left">
            {lines}
        </ul>
    }

    /* Слова */

    u00 = [
        [9, 255, 255, 255, 255, 255], // пусто
        [9,  10, 29,   30, 255, 255], // один
        [89, 90, 109, 255, 255, 255], // два
        [87, 92, 107, 255, 255, 255], // три
        [27, 32,   6,  13,  53,  66], // четыре
        [49, 50,  69,  70, 255, 255], // пять
        [28, 31,  48,  51,  68, 255], // шесть
        [47, 52,  67,  72, 255, 255], // семь
        [7,  12,  47,  52,  67,  72], // 8
        [8,  11,  71,  88,  91, 108], // 9
        [8,  11,  73,  86,  93, 106],  // 10
        [9,  10,  29,  30,      5, 14, 25, 34, 45, 54, 65 ], // 11
        [26, 33,  46,           5, 14, 25, 34, 45, 54, 65 ], // 12
        [87, 92, 107,           5, 14, 25, 34, 45, 54, 65 ], // 13
        [27, 32,   6,  13,  53, 5, 14, 25, 34, 45, 54, 65 ], // 14
        [49, 50,  69,           5, 14, 25, 34, 45, 54, 65 ], // 15
        [28, 31,  48,  51,      5, 14, 25, 34, 45, 54, 65 ], // 16
        [47, 52,  67,           5, 14, 25, 34, 45, 54, 65 ], // 17
        [7,  12,  47, 52,  67,  5, 14, 25, 34, 45, 54, 65 ], // 18
        [8,  11,  71, 88,  91,  5, 14, 25, 34, 45, 54, 65 ], // 19
        [26, 33,  14, 25,  34, 45, 54, 65], // 20
    ];
    u10 = [
        [], // 0
        [], // 10
        [36, 43,  56, 63,  77, 82, 97,102], // 20
        [ 3, 16,  23, 36,  77, 82, 97,102], // 30
        [64, 75,  84, 95, 104], // 40
        [76, 83,  96,103,  57, 62, 78, 81, 98], // 50
        [28, 31,  48, 51,  68, 57, 62, 78, 81, 98], // 60
        ];
    y00 = [
        [], // 0
        [35,36,37,42], // одна
        [26,33,46],     // две
        [ 3,16,23], // 3 (внизу)
        [], // 4
        [0,19,20,39], // пять
        ];
    y10 = [
        [], // 0
        [], // 10
        [26, 33,  14, 25,  34, 45, 54, 65], // 20 вверху
        [87, 92, 107, 25,  34, 45, 54, 65], // 30 вверху
        [64, 75,  84, 95, 104], // 40
        [49, 50,  69,  70, 255, 8, 11, 73, 86, 93], // 50
        ];
    _HLiterator = 0;  // just counter
    hl = [
        [4,15,24,35,44], // часов
        [74,85,94],      // час (справа)
        [74,85,94,105],  // часа
        ];
    ml = [
        [40,59,60,79,80],    // минут
        [40,59,60,79,80,100],// минута
        [40,59,60,79,80,99], // минуты
        ];
}