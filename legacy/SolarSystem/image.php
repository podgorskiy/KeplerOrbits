<?php
header("Content-Type: image/png");
$width=1024;
$height=1024;
$height2=512;
$colorr = array(255, 146, 36,  255, 255,  255,  36,   109, 109);
$colorg = array(0,   109, 182, 73,  109,  255,  255,  182, 109);
$colorb = array(170, 170, 255, 0,   0,    0,    170,  170, 170);
$size=$_GET['m'];
$date=$_GET['date'];
$time=$_GET['time'];
if (empty($size)) $size=100;
$im = imagecreatetruecolor($width, $height+$height2)
or die("Cannot Initialize new GD image stream");
$background_color = imagecolorallocate($im, 0, 0, 0);
$wight_color = imagecolorallocate($im, 255, 255, 255);
imagefill($im,0,0,$background_color);
$text_color = imagecolorallocate($im, 233, 14, 91);
imagestring($im, 1, 5, 5,  exec("date"), $text_color);
$grid_color = imagecolorallocate($im, 80, 80, 80);
$dark_color = imagecolorallocate($im, 40, 40, 40);
imageline($im, 0, $height/2, $width, $height/2, $grid_color);
imageline($im, $width/2, 0, $width/2, $height+$height2, $grid_color);
imageline($im, 0, $height, $width, $height, $wight_color);
imageline($im, 0, $height+$height2/2, $width, $height+$height2/2, $grid_color);
if (empty($date))
	if (empty($time))
		exec("./solarsystem",$result );
	else
		exec("./solarsystem -t $time",$result );
else {
	if (empty($time))
		exec("./solarsystem -d $date",$result );
	else
		exec("./solarsystem -d $date -t $time",$result );
}
$currentplanet = 0;
$firstpoint = 0;
$x1=0;
$y1=0;
$z1=0;
$x2=0;
$y2=0;
$z2=0;
for ($i=0;$i<10;$i++){
	$planet_color[$i+1] = imagecolorallocate($im, $colorr[$i], $colorg[$i], $colorb[$i]);	
}
$tr = imagecolorallocate($im, 200, 200, 200);
foreach($result as $index => $str){
	if (stripos($str,"planet")!=false) {
		$mas = explode(" ",$str);
		$currentplanet++;
		$plannetarrayname[$currentplanet] = $mas[2];
		$plannetarrayx[$currentplanet] = $mas[3];
		$plannetarrayy[$currentplanet] = $mas[4];
		$plannetarrayz[$currentplanet] = $mas[5];
		$firstpoint = 1;		
	} else {
		if ($currentplanet==0){
			imagestring($im, 1, 5, 50+10*$index, $str, $text_color);
		} else
		{
			$position = explode(" ",$str);
			$x1 = $position[1];
			$y1 = $position[2];
			$z1 = $position[3];
			if ($firstpoint==1){
				$firstpoint = 0;				
			} else {
				$y1_=$height/2-$y1/$size*$width;
				$y2_=$height/2-$y2/$size*$width;
				if (($y1_<$height)&($y2_<$height))
					imageline($im, $x1/$size*$width+$width/2, $y1_, $x2/$size*$width+$width/2, $y2_, $planet_color[$currentplanet]);
				$y1_=$height+$height2/2-$z1/$size*$width;
				$y2_=$height+$height2/2-$z2/$size*$width;
				if (($y1_>$height)&($y2_>$height))
					imageline($im, $x1/$size*$width+$width/2, $y1_, $x2/$size*$width+$width/2, $y2_, $planet_color[$currentplanet]);
			}
			$x2 = $x1;
			$y2 = $y1;
			$z2 = $z1;	
		}
	}
}

$rect = imagecolorallocate($im, 0, 0, 0);
for ($i=1;$i<10;$i++){
	$y1_=$height/2-$plannetarrayy[$i]/$size*$width;
	if ($y1_<$height) {
		imagefilledrectangle($im, $plannetarrayx[$i]/$size*$width+$width/2+2, $y1_+3,$plannetarrayx[$i]/$size*$width+$width/2+37,$y1_+11,$rect);
		imagefilledellipse($im, $plannetarrayx[$i]/$size*$width+$width/2, $y1_,5,5,$planet_color[$i]);
		imageellipse($im, $plannetarrayx[$i]/$size*$width+$width/2, $y1_,7,7,$grid_color);
		imagestring($im, 1, $plannetarrayx[$i]/$size*$width+$width/2+3, $y1_+3, $plannetarrayname[$i], $planet_color[$i]);
	}
	$y1_=$height+$height2/2-$plannetarrayz[$i]/$size*$width;
	if ($y1_>$height) {
		imagefilledrectangle($im, $plannetarrayx[$i]/$size*$width+$width/2+2, $y1_+3,$plannetarrayx[$i]/$size*$width+$width/2+37,$y1_+11,$rect);
		imagefilledellipse($im, $plannetarrayx[$i]/$size*$width+$width/2, $y1_,5,5,$planet_color[$i]);
		imageellipse($im,   $plannetarrayx[$i]/$size*$width+$width/2, $y1_,7,7,$grid_color);
		imagestring($im, 1, $plannetarrayx[$i]/$size*$width+$width/2+3, $y1_+3, $plannetarrayname[$i], $planet_color[$i]);
	}
}

imagefilledellipse($im, $width/2, $height/2,9,9,$planet_color[6]);
imageellipse($im,   $width/2, $height/2,11,11,$grid_color);
imagefilledellipse($im, $width/2, $height + $height2/2,9,9,$planet_color[6]);
imageellipse($im,   $width/2, $height + $height2/2,11,11,$grid_color);

$length = round($size/10.0,1);

imageline($im, 100, $height-100+2, 100, $height-100-2,$tr);
imageline($im, 100+$length/$size*$width, $height-100+2, 100+$length/$size*$width, $height-100-2,$tr);
imagefilledrectangle($im, 100, $height-100+2, 100+$length/$size*$width,$height-100+5,$tr);
imagestring($im, 1, 100+2, $height-100-10 , $length." AU", $tr);

imageline($im, 100, $height+$height2-100+2, 100, $height+$height2-100-2,$tr);
imageline($im, 100+$length/$size*$width, $height+$height2-100+2, 100+$length/$size*$width, $height+$height2-100-2,$tr);
imagefilledrectangle($im, 100, $height+$height2-100+2, 100+$length/$size*$width,$height+$height2-100+5,$tr);
imagestring($im, 1, 100+2, $height+$height2-100-10 , $length." AU", $tr);

imagepng($im);
imagedestroy($im);
?>