//var weekdays = ["Воскресенье","Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота",];
var weekdays = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday","Sunday"];
if(locale.weekdays !== undefined) weekdays = locale.weekdays;

$(document).ready(function(){
		window.wsSingleton = new Ws();
		window.wsSingleton.eventMessage = function(edata){
			
			if(edata.cmd == 'now'){
				// NOW()
				for(var k in edata.now){
					$('select#d'+k).val(edata.now[k]);
					}
				// Countdown values
				if($('#cy').val()==0){
					var now = edata.now;
					var d = new Date(now.y+2000,now.m-1,now.d,now.h,now.i,now.s);
					d.setTime(d*1 + edata.timer*1000);
					var cdt = {y:d.getFullYear()-2000,m:d.getMonth()+1,d:d.getDate(),h:d.getHours(),i:d.getMinutes(),s:d.getSeconds()}
						if(edata.timer > -1){
							console.log('Set cdt to ',cdt)
							for(var k in cdt) $('select#c'+k).val(cdt[k]);
						}else{
							console.log('Set cdt to ',edata.now)
							for(var k in edata.now) $('select#c'+k).val(edata.now[k]);
						}
					}
				}
				// timer
				if(edata.timer > -1){
					if($('#th').val()==0 && $('#ti').val()==0 && $('#ts').val() == 0) $('#timerFollow').prop('checked',true);
					var tfc = $('#timerFollow').prop('checked');
					if(tfc){
						var sec = edata.timer % 60; $("#ts").val(sec);
						var min = Math.floor(edata.timer % 3600 / 60); $("#ti").val(min);
						var hor = Math.floor(edata.timer / 3600); $("#th").val(hor);	
					}
				}else{
					$('#timerFollow').prop('checked',false);
				}
				// Countdown to, timer mirror

				// highlight mode
				var m = edata.mode;
				//console.log('mode',m)
				$('.i-mode:not([data-mode="'+m+'"])').removeClass('i-mode--current');
				$('.i-mode[data-mode="'+m+'"]:not(.i-mode--current)').addClass('i-mode--current');
			//console.log(edata)
			};

		$('.d').each(function(){
			var $this = $(this);
			for(var i = $this.attr('min'); i<= $this.attr('max'); i++){
				var val = (i>=2000) ? i-2000 : i;
				var txt = (i<10) ? '0'+i : i;
				if(this.id == 'dw'){
					txt = weekdays[i];
					}
				$('<option>',{text:txt,value:val}).appendTo($this);
				}
			});//.change(setSelectedTime);


		
			setInterval(function(){
				sendWS({cmd:"getNOW"});
			},500);
		
		//sendWS({cmd:"setDisplayMode",value:0x12});
		//
		$(document).on('click','.action_settime',sendNow);

	function setSelectedTime(){
		var data = {}
		var devDate = new Date();
		$('.d-time').each(function(){
			var k = this.id.substring(1,2);
			data[k] = $(this).val();
			});
		sendWS({
			cmd:"setTime",
			now:data
		});
		}
	
	function sendNow(){
		var devDate = new Date();
		sendWS({
			cmd:"setTime",
			now:{
				y: devDate.getFullYear() - 2000,
				m: devDate.getMonth() +1,
				d: devDate.getDate(),
				h: devDate.getHours(),
				i: devDate.getMinutes(),
				s: devDate.getSeconds(),
				w: devDate.getDay()
				}
			});
	}

	$(document).on("change" , ".d-time" ,setSelectedTime);
});

$(document).on("click" , ".action_timer_start" ,function(){
	var t = $('#th').val() * 3600 + $('#ti').val() * 60 + $('#ts').val() * 1;
	sendWS({cmd:"setTimer",s:t});
	$('#timerFollow').prop('checked',true).trigger('change');
});

$(document).on("click" , ".action_timer_stop" ,function(){
	sendWS({cmd:"setTimer",s:-1});
});

$(document).on("change" , "#timerFollow" ,function(){
	var $tf = $(this), tfc = $tf.prop('checked');
	$('.i-timer').each(function(i,el){
		if(tfc){
			$(el).data('v',$(el).val());
		}else{
			$(el).val($(el).data('v'));
		}
	});
});

var action_countdown_set = function(){

	var cdf = new Date(
		$('#dy').val()*1+2000,$('#dm').val()*1-1,$('#dd').val(),
		$('#dh').val(),$('#di').val(),$('#ds').val()
	);
	var cdt = new Date(
		$('#cy').val()*1+2000,$('#cm').val()*1-1,$('#cd').val(),
		$('#ch').val(),$('#ci').val(),$('#cs').val()
	);
	var diff = Math.floor((cdt-cdf)/1000);
	if(diff < 0){
		alert("Countdown to date must be in future");
		return false;
	}
	var maxdiff = 3600*97 - 1;
	if(diff > maxdiff){
		alert("Countdown to date is too big");
		return false;
	}
	// set interval into timer
	var sec = diff % 60; $("#ts").val(sec);
	var min = Math.floor(diff % 3600 / 60); $("#ti").val(min);
	var hor = Math.floor(diff / 3600); $("#th").val(hor);	

	console.log('Countdown to ',cdt,'diff',diff);
	return true;
}

$(document).on("click" , ".action_countdown_set" , action_countdown_set);

$(document).on("click" , ".action_countdown_start" ,function(){
	if(action_countdown_set()){
		$(".action_timer_start").trigger('click');
	}
	
});

