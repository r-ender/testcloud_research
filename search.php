<?php

$search= $_POST['searchwords'];

$searchoriginal= $search;

$search= trim($search);			//delete trailing whitespaces

$search= explode(" ", $search);		//separate input string into own elements

$countsearchterms= count($search);

echo "Originally inserted searchwords: "; 
echo $searchoriginal."\n";
echo "<br><br>";

//search is at the end of processing an array hence I must iterate over it
foreach ($search as $key=>$value) {
	$processed_searchwd[$key] = $value;
    echo("foreach iteration: key = $key; value = $value ---> processed_searchwd[$key] = $processed_searchwd[$key] <br>");
}

	//echo("processed_searchwd[0] = $processed_searchwd[0] ? <br>");

/*
echo "<br><br>";
echo "Nr. of searchwords: "; 
echo $countsearchterms."\n";
echo "<br><br>";
*/

switch($countsearchterms)
{
	case 1:
	$searchstring = "grep -r $processed_searchwd[0] | grep -v \"index.html\" | grep -v \"<h\"";
	break;

	case 2:
	$searchstring = "grep -r $processed_searchwd[0] | grep -r $processed_searchwd[1] | grep -v \"index.html\" | grep -v \"<h\"";
}


$shelltest=shell_exec($searchstring);

$expl_shellout = explode("\n", $shelltest);	//die Ausgabe in Zeilen unterteilen

$line_count = count($expl_shellout)-1;

foreach ($expl_shellout as $value) {
	
	$position = explode(":", $value);
	$linkresult= $position[0];
	$linkcontent = $position[1];

	echo ("<a href='$linkresult'>$linkcontent</a>"."<br>");
}
 

/*
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


Pseudo-Code:
1.) immer komplette Seite durchsuchen (später änderbar je nach Kategorie)
	---> grep -r /Dorlan/.... 

2.) Ergebnisse mit index.html in der directory löschen = grep -r Arbeitsleben | grep -v "index.html"


3.) bei grep -r ist die Ausgabe zeilenweise, also die Ausgabe unterteilen in Zeilen
*/
      
?>

