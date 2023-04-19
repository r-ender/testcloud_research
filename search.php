<?php

$search= $_POST['searchwords'];

$search= trim($search);			//delete trailing whitespaces - protection against xss

$searchoriginal= $search;

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

//echo("The number of lines is: $line_count <br>");

if ($line_count <= 1)
{

	$expl_shellout = implode($expl_shellout);
	$position = explode(":", $expl_shellout, 2);	
	$linkresult= $position[0];
	if( $position[1] == NULL){
		echo "Invalid search... redirecting to start page.";
		header( "Refresh:5; url=de/index.html", true, 303);
    		exit;
	}				
	$linkcontent = substr($position[1],0,250).'...'; 	//limit output to 250 chars
	
	echo ("<a href='$linkresult'>$linkcontent</a> <br><br>");
}
else
{
	foreach ($expl_shellout as $value) {
		
		$position = explode(":", $value, 2);
		$linkresult= $position[0];
		if( $position[1] == NULL) continue;			//break current iteration and make next iteration
		$linkcontent = substr($position[1],0,250).'...'; 	//limit output to 250 chars
		
		//doppelte ergebnisse herausfiltern ( bei grep werden sie untereinander angezeigt, also muss man nur erstes mit nächstem vergleichen)
		if ( $tmp_linkresult != $linkresult && !str_contains($linkcontent,"<img src=") ) 
			echo ("<a href='$linkresult'>$linkcontent</a> <br><br>");
		
		$tmp_linkresult = $linkresult;
	}
}

      
?>



