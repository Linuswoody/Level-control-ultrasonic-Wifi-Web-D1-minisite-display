
let Datum;
let Volumen = [4.3, 4.5, 3.9, 4.1, 4.6];
let Abstand;
let volume = [5.3, 4.5, 3.9, 4.1, 2.6];
let labelset1 = [1, 2, 3, 4, 5];
let voltage = [];
let time = [];

let volume_15 = [];
let time_15 = [];
let voltage_15 = [];

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
								
				for (let index = 0; index < volume.length; index+=(((Math.floor(volume.length/750))+1)*10)) {
					volume_15.push(volume[index]);
				}
				console.debug("volume_15", volume_15)

				for (let index = 0; index < re.time.length; index+=(((Math.floor(re.time.length/750))+1)*10)) {
					time_15.push(re.time[index]);
				}
				console.debug("time_15", time_15)

				for (let index = 0; index < re.voltage.length; index+=(((Math.floor(re.voltage.length/750))+1)*10)) {
					voltage_15.push(re.voltage[index]);
				}
				console.debug("voltage_15", voltage_15)

				let Diagramm = document.getElementById('myChart').getContext('2d');

				let Zisterne = new Chart(Diagramm, {
					type: 'bar',
					data: {
						labels: time_15,
						datasets: [{
							type: 'bar',
							label: 'Volumen in m3',
							data: volume_15,
							backgroundColor: 'rgba(54, 162, 235, 0.7)',
							borderColor: 'rgba(50, 150, 250, 1)',
							borderWidth: 1
						},	{
							type: 'line',
							label: 'Akkuspannung in V',
							data: voltage_15,
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
