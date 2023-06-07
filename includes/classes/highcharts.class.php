<?php
class highcharts {
	 private $chartName;
	 private $chartTitle;
	 private $chartRender;
     private $datatoShow;
     private $dataSet;
     private $charType;	 
     private $chart;
     private $Jur;
	 
	 function __construct($chartName,$charType,$chartTitle,$chartRender)
	 {
		$this->chartName=$chartName;
	 	$this->chartTitle=$chartTitle;
	 	$this->chartRender=$chartRender;	 	
	 	$this->charType=$charType;
	 	return '<script src="<?= secure_host ?>/media/highcharts.js" type="text/javascript"></script>
				 <script src="<?= secure_host ?>/media/modules/exporting.js" type="text/javascript"></script>';
	 }

	public function prepareData($dataSet, $dataTitle, $dataContent) {
		$this->dataSet = $dataSet;
		$this->Jur=array();
        foreach($this->dataSet as $k=>$v)
		{
            if (is_array ( $dataContent )) {
                foreach ( $dataContent as $k3 => $v3 ) {
                    if($v < 0){
                        $v=0;
                    }

                        if($dataContent[$k3] == $k) $this->Jur[$dataContent["$k3"]]=$this->Jur[$dataContent["$k3"]]+$v;
                }
            } else {
                if($v["$dataContent"] < 0)
                {
                    $v["$dataContent"]=0;
                }
                    $this->Jur[$v["$dataTitle"]]=$this->Jur[$v["$dataTitle"]]+$v["$dataContent"];
            }
		}
		$this->dataSet=$this->Jur;

	foreach ( $this->dataSet as $k => $v ) {
			$this->datatoShow .= "['" . $k.  "'," . $v . "],";
		}
	}

	 public function showChart($size='300') 
	 {
	 $this->chart = "<script type='text/javascript'>
	 $(document).ready(function() {
	 	 		$this->chartName= new Highcharts.Chart({
	 			chart: {
	 				renderTo: '$this->chartRender',
	 				plotBackgroundColor: null,
	 				plotBorderWidth: null,
	 				plotShadow: false
	 			},
	 			credits: {
                  enabled: false
                },
	 			title: {
	 				text: '".$this->chartTitle."'
	 			},
	 			tooltip: {
	 				formatter: function() {
	 					return '<b>'+ this.point.name +'</b>: '+ (this.y).toFixed(2);
	 				}
	 			},
	 			plotOptions: {
	 				pie: {
	 					allowPointSelect: true,
	 					cursor: 'pointer',
	 					size: $size,
	 					dataLabels: {
	 						enabled: true,
	 						color: '#000000',
	 						connectorColor: '#000000',
	 						formatter: function() {
	 							if(this.point.name == 'playtech_rake') {
	 								var name = 'playt./skywind_rake';
	 							}
	 							else {
	 								name = this.point.name;
	 							}
	 							return '<b>'+ name +'</b>: '+ (this.percentage).toFixed(2) +' %';
	 						}
	 					}
	 				}
	 			},
	 			series: [{
	 				type: '".$this->charType."',
	 				name: 'Rake',
	 				pointWidth:15,
	 				data: [$this->datatoShow]
	 		}]
	 	});
	 	});
	 	  </script>";
	 	return  $this->chart;
	 }
}
?>