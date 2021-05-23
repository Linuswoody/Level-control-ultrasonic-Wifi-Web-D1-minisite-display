
let Datum;
let Volumen = [4.3, 4.5, 3.9, 4.1, 4.6];
let Abstand;
let volume = [5.3, 4.5, 3.9, 4.1, 2.6];
let labelset1 = [1, 2, 3, 4, 5];
let voltage = [];
let time = [];

let volume50 = [];
let time50 = [];
let voltage50 = [];

function getData(){
	fetch('ZisternenHistoryTime.txt')
	.then(function(res){
		return res.text();
	})
	.then(function(res){
		return res.split('\n');
	})
	.then(time => {
		fetch('ZisternenHistoryAkku.txt')
		.then(function(res){
			return res.text();
		})
		.then(function(res){
			let voltage = res.split('\n');
			return { time: time, voltage: voltage }
		})
		.then(re => {
			fetch('ZisternenHistoryData.txt')
			.then(function(res){
				return res.text();
			})
			.then(function(res){
				return res.split(' \n');
			})
			.then(function(result){
				volume = result;
				console.debug("time", re.time)
				console.debug("voltage", re.voltage)
				console.debug("volume", volume)
								
				for (let index = volume.length - 50; index < volume.length; index++) {
					volume50.push(volume[index]);
				}
				console.debug("volume50", volume50)

				for (let index1 = re.time.length - 50; index1 < re.time.length; index1++) {
					time50.push(re.time[index1]);
				}
				console.debug("time50", time50)

				for (let index2 = re.voltage.length - 50; index2 < re.voltage.length; index2++) {
					voltage50.push(re.voltage[index2]);
				}
				console.debug("voltage50", voltage50)

				let Diagramm = document.getElementById('myChart').getContext('2d');

				let Zisterne = new Chart(Diagramm, {
					type: 'bar',
					data: {
						labels: time50,
						datasets: [{
							type: 'bar',
							label: 'Volumen in m3',
							data: volume50,
							backgroundColor: 'rgba(54, 162, 235, 0.7)',
							borderColor: 'rgba(50, 150, 250, 1)',
							borderWidth: 1
						},	{
							type: 'line',
							label: 'Akkuspannung in V',
							data: voltage50,
							yAxisID: "bar-y-axis",
							backgroundColor: 'rgba(100, 245, 162, 0.3)',
							borderColor: 'rgba(50, 150, 250, 1)',
							borderWidth: 1
							}
						]
					},
					
					options: {
						title: {
							display: true,
							text: "Füllvolumen und Akkuspannung"
						},
						scales: {
							xAxes: [{
								stacked: false,
							}],  
							yAxes: [{
								stacked: false,
								position: 'left',
								ticks: {
									beginAtZero: true,
									min: 0,
                					max: 6
								}
							},
							{
								id: "bar-y-axis",
								stacked: false,
								display: true, //optional
								position: 'right',
								ticks: {
								  beginAtZero: true,
								  min: 3.0,
								  max: 3.4
								},
								type: 'linear'
							  }]
						}
					}
				});
			})
		})
	})

	//.then((res) => res.text())
	//.then((data) => {
	//  document.getElementById('output').innerHTML = data;
	//})
	//.then(volume=data.split(\r\n))
	.catch((err) => console.error(err))
}

getData();
