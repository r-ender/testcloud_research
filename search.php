<?php

$search= $_POST['searchwords'];

$searchoriginal= $search;

$search= strtolower($search);		//make all searchwords lower case

$search= trim($search);			//delete trailing whitespaces

$search= explode(" ", $search);		//separate input string into own elements

$countsearchterms= count($search);

echo "Originally inserted searchwords: "; 
echo $searchoriginal."\n";
echo "<br><br>";

//search is at the end of processing an array hence I must iterate over it
echo "Processed searchwords: "; 
foreach ($search as $value) {
	$processed_searchwd = $value;
    echo $processed_searchwd, "\n";
}

echo "<br><br>";
echo "Nr. of searchwords: "; 
echo $countsearchterms."\n";
echo "<br><br>";

echo "root search directory: ";
echo getcwd()."\n";
echo "<br><br>";	

//$searchstring = 'grep -r $processed_searchwd | grep -v \"index.html\" | grep -v \"<h\"';
$searchstring = "grep -r $processed_searchwd | grep -v index.html | grep -v <h";	// ---> klappt noch nicht, Rest passt!

//$shelltest=shell_exec('grep -r $procesed_searchwd | grep -v "index.html" | grep -v "<h"');
$shelltest=shell_exec($searchstring);

echo $shelltest;

$expl_shellout = explode("\n", $shelltest);	//die Ausgabe in Zeilen unterteilen

$line_count = count($expl_shellout)-1;

echo "exploded Shelltest gives $line_count results: <br>";
foreach ($expl_shellout as $value) {
	
	$position = explode(":", $value);
	$linkresult= $position[0];
	$linkcontent = $position[1];

	//echo "Link zum Ergebnis: $linkresult <br>";
	//echo "Link zum Inhalt: $linkcontent <br>";

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

