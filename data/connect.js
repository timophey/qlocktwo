class Ws {
	  get newClientPromise() {
		return new Promise((resolve, reject) => {

		var host = window.location.hostname;
		//console.log(window.location.hostname,window.globalhost)
		if(host == 'localhost' && window.globalhost !== undefined){
			host = window.globalhost;
			}
		

		  let wsClient = new WebSocket("ws://"+host+":81");
		  console.log(wsClient)
		  wsClient.onopen = () => {
			console.log("connected");
			resolve(wsClient);
		  };
		  wsClient.onerror = error => reject(error);
		  /*
		  if(onWSevent !== undefined){
			  wsClient.onmessage = evt => {
				  onWSevent(evt);
				  };
			  }*/
		})
	  }
	  
	  get clientPromise() {
		if (!this.promise) {
		  this.promise = this.newClientPromise
		}
		return this.promise;
	  }
	  
	  set eventMessage(h){
		  window.wsSingleton.clientPromise
			.then( wsClient =>{
				wsClient.onmessage = function(evt){
					var data = JSON.parse(evt.data);
					return h(data);
					}
			//console.log('sended')
		  })
	  .catch( error => alert(error) )
		}
		  
	}
	
	function sendWS(data){
			window.wsSingleton.clientPromise
	  .then( wsClient =>{wsClient.send(
		  JSON.stringify(data)
		  ); 
		//  console.log('sended');
		})
	  .catch( error => function(error){
	  	console.error(error);
	  })
	}

