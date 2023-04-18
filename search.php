<?php

$search= $_POST['searchwords'];

$searchoriginal= $search;

$search= trim($search);			//delete trailing whitespaces

$search= explode(" ", $search);		//separate input string into own elements

$countsearchterms= count($search);

echo "Inserted searchwords: "; 
echo $searchoriginal."\n";
echo "<br><br>";

//search is at the end of processing an array hence I must iterate over it
foreach ($search as $key=>$value) {
	$processed_searchwd[$key] = $value;
    //echo("foreach iteration: key = $key; value = $value ---> processed_searchwd[$key] = $processed_searchwd[$key] <br>");
}

if ($processed_searchwd[0] == NULL) $countsearchterms = 0;

/*
echo "<br><br>";
echo "Nr. of searchwords: "; 
echo $countsearchterms."\n";
echo "<br><br>";
*/

switch($countsearchterms)
{
	case 1:
	$searchstring = "grep -ir $processed_searchwd[0] | grep -v \"index.html\" | grep -v \"<h\"";
	break;

	case 2:
	$searchstring = "grep -ir $processed_searchwd[0] | grep -ir $processed_searchwd[1] | grep -v \"index.html\" | grep -v \"<h\"";
	break;
	
	case 3:
	$searchstring = "grep -ir $processed_searchwd[0] | grep -ir $processed_searchwd[1] | | grep -ir $processed_searchwd[2] | grep -v \"index.html\" | grep -v \"<h\"";
	break;
	
	case 4:
	$searchstring = "grep -ir $processed_searchwd[0] | grep -ir $processed_searchwd[1] | grep -ir $processed_searchwd[2] | grep -ir $processed_searchwd[3] | grep -v \"index.html\" | grep -v \"<h\"";
	break;
	
	default:
	echo "Invalid search... redirecting to start page.";
	header( "Refresh:5; url=de/index.html", true, 303);
    	exit;
}


$shelltest=shell_exec($searchstring);

$expl_shellout = explode("\n", $shelltest);	//die Ausgabe in Zeilen unterteilen

$line_count = count($expl_shellout)-1;

if ($countsearchterms == 1)
{
	echo("expl_shellout:<br>$expl_shellout <br>");
	$position = explode(":", $expl_shellout, 2);	//PHP Warning:  explode() expects parameter 2 to be string, array given in /var/www/dorlan/search.php on line 66

	echo("position:<br>$position <br>");
	$linkresult= $position[0];
	$linkcontent = $position[1];
	
	echo("$linkresult --- $linkcontent <br>");

	echo ("<a href='$linkresult'>$linkcontent</a> <br><br>");
}
else
{
	foreach ($expl_shellout as $value) {
		
		$position = explode(":", $value, 2);
		$linkresult= $position[0];
		$linkcontent = $position[1];
		
		//doppelte ergebnisse herausfiltern ( bei grep werden sie untereinander angezeigt, also muss man nur erstes mit nächstem vergleichen)
		if ( $tmp_linkresult != $linkresult) echo ("<a href='$linkresult'>$linkcontent</a> <br><br>");
		$tmp_linkresult = $linkresult;
	}
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

4.) groß-/kleinschreibung ignorieren

5.) doppelte Ergebnisse herausfiltern

6.) bloß bei dem ersten doppelpunkt array teilen  (explode, implode) ---> explode(' ', $your_string, 2)
 
*/
      
?>


