<?php

$directory = "Articles/";


if ($submitbutton){
if (!empty($searchoriginal)) 
{
if (is_dir($directory)){

  if ($open = opendir($directory)){
if ($countsearchterms == 1)
{
    while (($file = readdir($open)) !== false){
	$fileoriginal= $file;
	$file= strtolower($file);
	$file= str_replace("-", " ", $file);
	$file= str_replace("_", " ", $file);
	$position= strpos("$file", ".");
	$file= substr($file, 0, $position);
	
      if (strpos("$file",  "$search[0]") !== false)
	{
	$file= ucwords($file);
	$array[] += "$file";
	 echo "<a href='http://www.learningaboutelectronics.com/$directory" . "$fileoriginal'>$file</a>"."<br>";
}
 
	
    }
}
else if ($countsearchterms == 2) {
while (($file = readdir($open)) !== false){
$fileoriginal= $file;
	$file= strtolower($file);
	$file= str_replace("-", " ", $file);
	$position= strpos("$file", ".");
	$file= substr($file, 0, $position);
      if ((strpos("$file",  "$search[0]") !== false) && (strpos("$file",  "$search[1]") !== false))
	{
	$file= ucwords($file);
	$array[] += "$file";
	 echo "<a href='http://www.learningaboutelectronics.com/$directory" . "$fileoriginal'>$file</a>"."<br>";
}
 
	
    }
    

}

else if ($countsearchterms == 3) {
while (($file = readdir($open)) !== false){
$fileoriginal= $file;
	$file= strtolower($file);
	$file= str_replace("-", " ", $file);
	$position= strpos("$file", ".");
	$file= substr($file, 0, $position);
      if ((strpos("$file",  "$search[0]") !== false) && (strpos("$file",  "$search[1]") !== false) && (strpos("$file",  "$search[2]") !== false))
	{
	$file= ucwords($file);
	$array[] += "$file";
	 echo "<a href='http://www.learningaboutelectronics.com/$directory" . "$fileoriginal'>$file</a>"."<br>";
}
 
	
    }
    

}

else if ($countsearchterms == 4) {
while (($file = readdir($open)) !== false){
$fileoriginal= $file;
	$file= strtolower($file);
	$file= str_replace("-", " ", $file);
	$position= strpos("$file", ".");
	$file= substr($file, 0, $position);
      if ((strpos("$file",  "$search[0]") !== false) && (strpos("$file",  "$search[1]") !== false) && (strpos("$file",  "$search[2]") !== false)&& (strpos("$file",  "$search[3]") !== false))
	{
	$file= ucwords($file);
	$array[] += "$file";
	 echo "<a href='http://www.learningaboutelectronics.com/$directory" . "$fileoriginal'>$file</a>"."<br>";
}
 
	
    }
    

}

else if ($countsearchterms == 5) {
while (($file = readdir($open)) !== false){
$fileoriginal= $file;
	$file= strtolower($file);
	$file= str_replace("-", " ", $file);
	$position= strpos("$file", ".");
	$file= substr($file, 0, $position);
      if ((strpos("$file",  "$search[0]") !== false) && (strpos("$file",  "$search[1]") !== false) && (strpos("$file",  "$search[2]") !== false)&& (strpos("$file",  "$search[3]") !== false)
&& (strpos("$file",  "$search[4]") !== false))
	{
	$file= ucwords($file);
	$array[] += "$file";
	 echo "<a href='http://www.learningaboutelectronics.com/$directory" . "$fileoriginal'>$file</a>"."<br>";
}
}  
}
else if ($countsearchterms == 6) {
while (($file = readdir($open)) !== false){
$fileoriginal= $file;
	$file= strtolower($file);
	$file= str_replace("-", " ", $file);
	$position= strpos("$file", ".");
	$file= substr($file, 0, $position);
      if ((strpos("$file",  "$search[0]") !== false) && (strpos("$file",  "$search[1]") !== false) && (strpos("$file",  "$search[2]") !== false)&& (strpos("$file",  "$search[3]") !== false)
&& (strpos("$file",  "$search[4]") !== false) && (strpos("$file",  "$search[5]") !== false))
	{
	$file= ucwords($file);
	$array[] += "$file";
	 echo "<a href='http://www.learningaboutelectronics.com/$directory" . "$fileoriginal'>$file</a>"."<br>";
}
}  
}
else if ($countsearchterms == 7) {
while (($file = readdir($open)) !== false){
$fileoriginal= $file;
	$file= strtolower($file);
	$file= str_replace("-", " ", $file);
	$position= strpos("$file", ".");
	$file= substr($file, 0, $position);
      if ((strpos("$file",  "$search[0]") !== false) && (strpos("$file",  "$search[1]") !== false) && (strpos("$file",  "$search[2]") !== false)&& (strpos("$file",  "$search[3]") !== false)
&& (strpos("$file",  "$search[4]") !== false) && (strpos("$file",  "$search[5]") !== false) && (strpos("$file",  "$search[6]") !== false))
	{
	$file= ucwords($file);
	$array[] += "$file";
	 echo "<a href='http://www.learningaboutelectronics.com/$directory" . "$fileoriginal'>$file</a>"."<br>";
}
}  
}
closedir($open);
    }

  }//while loop

$arraycount= count($array);

if ($arraycount == 0)
{
echo "No results for this search entered";
}
}
}
?>
