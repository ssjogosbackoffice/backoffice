<?php


function AddColumn($text, $width, $align, $name = "") {
	global $table_columns, $table_curcol;
	
	if ($table_curcol == 0)
		echo "<tr valign='top'>";
	
	echo "<td width='" . $width . "' align='" . $align . "'>" . $text . "</td>";

	$table_curcol++;
	if ($table_curcol == $table_columns) {
		$table_curcol = 0;
		echo "</tr>";
	}
}

function AddEditColumnJS($text, $width, $align, $bgcolor, $jscript, $name = "") {
	global $table_columns, $table_curcol;
	
	if ($table_curcol == 0)
		echo "<tr valign='top'>";
	
	//    echo "<td width='".$width."' align='".$align."'>".$text."</td>";
	echo "<td width='" . $width . "' align='" . $align . "' bgcolor='" . $bgcolor . "'>";
	echo "<input id='editfield' type='text' name='" . $name . "' value=\"$text\" onclick=\"return " . $jscript . "(" . $text . ");\">";
	echo "</td>";
	
	$table_curcol++;
	if ($table_curcol == $table_columns) {
		$table_curcol = 0;
		echo "</tr>";
	}
}

function AddEditColumn($text, $width, $align, $bgcolor, $name = "", $jscript = "") {
	global $table_columns, $table_curcol;
	$size = floor($width / 5);
	
	if ($table_curcol == 0)
		echo "<tr valign='top'>";
	
	//    echo "<td width='".$width."' align='".$align."'>".$text."</td>";
	echo "<td width='" . $width . "' align='" . $align . "' bgcolor='" . $bgcolor . "'>";
	echo "<input id='" . $name . "' type='text' name='" . $name . "' value=\"$text\" size='" . $size . "' onchange='" . $jscript . "(this);'>";
	echo "</td>";
	
	$table_curcol++;
	if ($table_curcol == $table_columns) {
		$table_curcol = 0;
		echo "</tr>";
	}
}

// AddColumn2( $row[1], 250, "left", "white");
function AddColumn2($text, $width, $align,$bgcolor)
{
    global $table_columns, $table_curcol;

    if ( $table_curcol==0)
        echo "<tr valign='top'>";

    echo "<td width='".$width."' align='".$align."' bgcolor='".$bgcolor."'>".$text."</td>";

    $table_curcol++;
    if ( $table_curcol == $table_columns) 
    {
        $table_curcol=0;
        echo "</tr>";
    }
}

function AddColumnImg($image, $width, $align)
{
    global $table_columns, $table_curcol;

    if ( $table_curcol==0)
        echo "<tr valign='top'>";

    echo "<td width='".$width."' align='".$align."'><img src='".$image."'/></td>";

    $table_curcol++;
    if ( $table_curcol == $table_columns) 
    {
        $table_curcol=0;
        echo "</tr>";
    }
}

function EndTable() {
	print "</tbody>";
    echo "</table>";
}

function MessageBox($text, $icon) {
	echo "<table class='msgbox' cellspacing='1' cellpadding='1'>\n";
	echo "<tr>\n";
	echo "<td width='60'>\n";
	echo "<img src='icon/msg_" . $icon . ".jpg'></td>\n";
	echo "<td>" . $text . "</td>\n";
	echo "</tr></table>\n";
	echo "<br/>\n";
}

function FormInit($title="", $formcode, $name=null, $action="#") {
	echo "<form name='$name' method='post' enctype='multipart/form-data' action='$action'>\n";
	echo "<input type='hidden' name='formcode' value=\"$formcode".".php\" />\n";
	
	if($title && $title != "") {
		echo "<table border='0' cellspacing='2' cellpadding='2' id='$name' class='form-table'>\n\t";
		print "<thead>\n\t<tr valign='center'>\n\t";
		echo "<th colspan='2'> &nbsp;$title </th>\n";
        echo "</tr>\n</thead>\n";
        print "<tbody>\n\t";
	}
}

function FormHeader($title, $class=null) {
	if($class != null) {
		$class = " class='$class'";
	}
	echo "<tr height='22' valign='center' ".$class.">";
	echo "<td colspan='2' style='background-color:#334477; border-width:1px; border-top-color:rgb(102,102,102); border-right-color:white; border-bottom-color:white; border-left-color:rgb(102,102,102); border-style:solid;'>";
	echo "<font color='white'><b>&nbsp;" . $title . "</b></font>";
	echo "</td></tr>\n";
	//echo "<tr height='20'><td colspan='2' bgcolor='#003399'><font color='white'><b>&nbsp;".$title."</b></font></td></tr>\n";
}

function FormEnd() {
	echo "</tbody></table></form>\n";
}

function FormTextBoxSwitch($label, $field, $length, $value = "") {
	if ($_SESSION["muser_level"] < 10)
		FormTextBox($label, $field, $length, $value);
	else
		FormStaticBox($label, $field, $length, $value);
}

function FormTextBox($label, $field, $length, $value="", $class="", $size="") {
	if (($value != "") && (isset ($_REQUEST[$field])))
		$value = $_REQUEST[$field];
	
	$class = ($class != "" ? "class='$class'" : "");
	$size = ($size != "" ? "size='$size'" : "");
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	echo "<td><input id='in$length' type='text' name='$field' maxlength='$length' $size value=\"$value\" ".$class." ></td></tr>\n";
}

function FormTextBoxSearch($label, $field, $length, $value = "") {
	if (($value <= "") && (isset ($_REQUEST[$field])))
		$value = $_REQUEST[$field];
	
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	echo "<td>";
	echo "<input id='in" . $length . "' type='text' name='" . $field . "' maxlength='" . $length . "' value=\"" . $value . "\">" ;
	echo "&nbsp; <select name=\"".$field."_custom\">";
	if (isset($_REQUEST[$field."_custom"]) && ($_REQUEST[$field."_custom"]=="contains"))
		$selcont="selected";
	else
		$selcont='';
	echo "<option value=\"contains\" ".$selcont.">Contains</option>";
	if (isset($_REQUEST[$field."_custom"]) && ($_REQUEST[$field."_custom"]=="equal"))
		$selequ="selected";
	else
		$selequ='';
	echo "<option value=\"equal\" ".$selequ.">Is equal to</option>";
	echo "</select>";
	echo "</td></tr>\n";
}

function FormStaticBox($label, $field, $length, $value = "") {
	//$value = "";
	if ($value != "" && isset($_REQUEST[$field]))
		$value = $_REQUEST[$field];
	
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	//echo "<td><input id='in".$length."' readonly='true' type='text' name='".$field."' maxlength='".$length."' value='".$value."'></td></tr>\n";    
	echo "<td><b>" . $value . "</b></td></tr>\n";
	
	if (substr($value, 0, 1) != "<")
		echo "<input type='hidden' name='" . $field . "' value=\"" . $value . "\">";
}

function FormStaticBoxRight($label, $field, $length, $value = "") {
	if ($value <= "")
		$value = $_REQUEST[$field];
	
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	echo "<td width='" . (10 * $length) . "' align='right'>" . $value . "</td></tr>\n";
	echo "<input type='hidden' name='" . $field . "' value=\"" . $value . "\">";
}

function FormLinkRight($label, $field, $length, $value = "") {
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	echo "<td width='" . (10 * $length) . "' align='right'><a href='index.php?m=" . $value . "'>" . $field . "</a></td></tr>\n";
}

function FormImageLinkRight($label, $field, $width, $height, $value = "", $imageId) {
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	echo "<td align='right'>";
	echo "<img src='avatar/" . $imageId . ".bmp' width='" . $width . "' height='" . $height . "'><br>";
	echo "<a href='index.php?m=" . $value . "'>" . $field . "</a></td></tr>\n";
}

function FormImageRight($label, $field, $width, $height, $value = "", $imageId = "0") {
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	echo "<td align='right'>";
	echo "<img src='avatar/" . $imageId . ".bmp' width='" . $width . "' height='" . $height . "'></td></tr>\n";
}

function FormHidden($field, $value=null) {
	if ($value == "" || $value === null)
		if(isset($_REQUEST[$field])) {
			$value = $_REQUEST[$field];
		}

	echo "<input type='hidden' name='" . $field . "' value=\"" . $value . "\">";
}

function FormTextArea($label, $field, $length, $cols, $rows, $value = "") {
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	echo "<td><textarea name='" . $field . "' wrap='soft' cols='" . $cols . "' rows='" . $rows . "' maxlength='" . $length . "' enabled='true'>" . $value . "</textarea></td></tr>\n";
}

function FormTextArea2($label, $field, $value, $length, $cols, $rows) {
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	echo "<td><textarea name='" . $field . "' wrap='soft' cols='" . $cols . "' rows='" . $rows . "' maxlength='" . $length . "' enabled='true'>" . $value . "</textarea></td></tr>\n";
}

function FormNote($label, $source, $id) {
	global $lang;
	$my_lvl = false;
	if ($_SESSION["muser_level"] < 10) {
		$readlvl = array (
			1,
			2,
			3,
			11,
			12,
			13,
			14
		);
		foreach ($readlvl as $lvl) {
			$file = $source . "/" . $id . "/note" . $lvl . ".txt";
			if (file_exists($file)) {
				$fp = fopen($file, "r");
				$value = fread($fp, 64000);
				fclose($fp);
				
				if ($value > "") {
					echo "<tr><td class='fieldlabel'>" . $lang["level" . $lvl] . "&nbsp;&nbsp;</td>";
					echo "<td><textarea name='note" . $lvl . "' wrap='soft' cols='38' rows='12' readonly='true'>" . $value . "</textarea></td></tr>\n";
					
					if ($lvl == $_SESSION["muser_level"])
						$my_lvl = true;
				}
			}
		}
	}
	
	if ($my_lvl == false) {
		$lvl = $_SESSION["muser_level"];
		$file = $source . "/" . $id . "/note" . $lvl . ".txt";
		if (file_exists($file)) {
			$fp = fopen($file, "r");
			$value = fread($fp, 64000);
			fclose($fp);
		} else
			$value = "";
		
		echo "<tr><td class='fieldlabel'>" . $label . "<br>" . $lang["level" . $lvl] . "&nbsp;&nbsp;</td>";
		echo "<td><textarea name='note" . $lvl . "' wrap='soft' cols='38' rows='12' enabled='true'>" . $value . "</textarea></td></tr>\n";
	}
	
}

function FormPlayerNote($label, $source, $id) {
	$file = $source . "/" . $id . "/note.txt";
	$value  = null;
	
	if (file_exists($file)) {
		$fp = fopen($file, "r");
		$value = fread($fp, 64000);
		fclose($fp);
	}
	
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	echo "<td><textarea name='note' wrap='soft' cols='38' rows='12' enabled='true'>" . $value . "</textarea></td></tr>\n";
}

function FormPassword($label, $field, $length) {
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	echo "<td><input id='in" . $length . "' type='password' name='" . $field . "' maxlength='" . $length . "'></td></tr>\n";
}

function FormCheckBox($label, $field, $checked) {
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	echo "<td><input type='checkbox' name='" . $field . "' value='1' ";
	if ($checked != "" && $checked != "0" && $checked != 0)
		echo " checked='checked' >";
	else
		echo ">";
	
	echo "&nbsp;</td></tr>\n";
}

function FormDate($label, $field, $value) {
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	
	echo "<td><input type='text' name='" . $field . "' id='" . $field . "' maxlength='16' size='18' value=\"$value\" />";
	echo "<a href=\"javascript:void(0);\" onclick=\"displayCalendar(document.getElementById('".$field."'),'dd-mm-yyyy hh:ii',this,true)\">" .
			"<img src=\"images/cal.gif\" width=\"16\" height=\"16\" border=\"0\" alt=\"Click Here to Pick up the date\"></a>";
	echo "</td></tr>";
}

function FormRadioButton($label, $field, $value) {
	global $lang;
	
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	echo "<td>";
	echo "<input type='radio' name='" . $field . "' value=1 ";
	if ($value == 1)
		echo "checked";
	echo "> " . $lang["male"];
	echo "<input type='radio' name='" . $field . "' value=2 ";
	if ($value == 2)
		echo "checked";
	echo "> " . $lang["female"];
	echo "</td></tr>";
}

function FormRadioGroup() {
	global $lang;
	
	$label = func_get_arg(0);
	$field = func_get_arg(1);
	
	$value = 1;
	if (isset ($_REQUEST[$field]))
		$value = $_REQUEST[$field];
	
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	echo "<td>";
	
	$index = 1;
	for ($i = 2; $i < func_num_args(); $i += 2) {
		if (func_get_arg($i +1) > "")
			echo "<div id='" . $field . $index . "' >";
		else
			echo "<div id='" . $field . $index . "' style='display: none;'>";
		
		echo "<input type='radio' name='" . $field . "' value=\"$index\" ";
		if ($value == $index)
			echo " checked";
		
		echo ">" . func_get_arg($i) . "&nbsp;&nbsp;&nbsp;&nbsp;</input></div>";
		$index++;
	}
	echo "</td></tr>";
}

function TBodyStart($id, $show) {
	//echo "<tbody id='".$id."' style='display: block;'>";
	
	if ($show > "")
		echo "<tbody id='" . $id . "' style='display: table-row-group;'>";
	else
		echo "<tbody id='" . $id . "' style='display: none;'>";
}

function TBodyEnd() {
	echo "</tbody>";
}

function FormListBoxDB($label, $field, $query, $jscript="", $showselall="", $selwhat=null) {
	global $dbLink;
	
	$first_id = 0;
	$index = 0;
	
	if(isset($_REQUEST[$field])) {
		$index = $_REQUEST[$field];
	}
	
	if($selwhat != null) {
		$index = $selwhat;
	}
	
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	
	if ($jscript != "") {
		echo "<td><select id='".$field."' name='".$field."' size='1' onchange='".$jscript."(this);'>\n";
	} else {
		echo "<td><select id='".$field."' name='".$field."' size='1'>\n";
	}
	
	$result = mysql_query($query, $dbLink);

    if($result) {
	    $s = "";
        if($showselall != "") {
            $s = ($selwhat == null) ? "selected='selected'" : "";
        }
		echo "<option $s value=\"0\">$showselall</option>\n";
		
		while($row = mysql_fetch_row($result)) {
			echo "<option ";
			if ((string)$row[0] == (string)$index) {
				$first_id = $row[0];
				echo "selected ";
			} else
				if ($first_id == 0)
					$first_id = $row[0];
				
			echo "value=\"$row[0]\">" . $row[1] . "</option>\n";
		}
		
		mysql_free_result($result);
	}
	
	echo "</select></td></tr>\n";
	return $first_id;
}

function FormListBoxDB2($label, $field, $query, $jscript = "", $index = 0, $showselall = "") {
	global $dbLink;
	$first_id = 0;
	
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	
	if ($jscript > "")
		echo "<td><select id='" . $field . "' name='" . $field . "' size='1' onchange='" . $jscript . "(this);'>\n";
	else
		echo "<td><select id='" . $field . "' name='" . $field . "' size='1'>\n";
	
	$result = mysql_query($query, $dbLink);
	if ($result) {
		if ($showselall > "")
			echo "<option selected value='0'>" . $showselall . "</option>\n";
		
		while (($row = mysql_fetch_row($result))) {
			echo "<option ";
			if ($row[0] == $index) {
				$first_id = $row[0];
				echo "selected ";
			} else
				if ($first_id == 0)
					$first_id = $row[0];
				
			echo "value=\"$row[0]\">" . $row[1] . "</option>\n";
		}
		
		mysql_free_result($result);
	}
	echo "</select></td></tr>\n";
	return $first_id;
}
function FormListTextJSON($label, $field, $phpcallback, $length, $id_input, $suggestlimit, $value)
	{
	// as top as possible you'd have to add the strings below: (possibly without rem! )
	//<script type="text/javascript" src="../script/bsn.AutoSuggest_2.1.3.js" charset="utf-8"></script>
	//<link rel="stylesheet" href="../css/autosuggest_inquisitor.css" type="text/css" media="screen" charset="utf-8" />
	
	
	//$label is the label for the text input;
	//$field is the field name. Should be better if you assign the exact DBColumn denomination.
	//$phpcallback is the file name of the script that contains query and compose the json array
	//$length is the maxlength of the input
	//$id_input is the value of "id" for the input
	//$suggestlimit is the int to define the max number of results to suggest
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	echo "<td><input id='".$id_input."' type='text' name='" . $field . "' value=\"".$value."\">";
	
	echo "<script type=\"text/javascript\">
	var options = {
	script:\"".$phpcallback."?\",
	varname:\"input\",
	json:true,
	shownoresults:true,
	maxresults:50
	};
	var as_json = new bsn.AutoSuggest('".$id_input."', options);
	
	</script>";
	echo "</td></tr>";
	}

function FormListBox($label, $field, $rows, $jscript = "", $value = "", $width = "") {
	echo "<tr><td class='fieldlabel'>" . $label . "&nbsp;&nbsp;</td>";
	
	if ($width != "") {
		$width = "style='width:" . $width . "px;' ";
	}
	
	if ($jscript != "")
		echo "<td><select id='" . $field . "' name='" . $field . "' size='" . $rows . "' value=\"$value\" " . $width . " onchange='" . $jscript . "(this);'>\n";
	else
		echo "<td><select id='" . $field . "' name='" . $field . "' size='" . $rows . "' value=\"$value\" " . $width . ">\n";
}

function FormListBoxEnd() {
	echo "</select>&nbsp;</td></tr>";
}

function FormListItem($name, $value, $selected, $addinputs=null) {
	echo "<option ";
	if ($selected != "")
		echo " selected ";
	
	echo "value=\"$value\" $addinputs >" . $name . "</option>";
}

function AddColumnButton($value1, $name, $jscript = "") {
	echo "<td colspan='2' align='center'>";
	if ($jscript > "")
		echo "<input id='" . $name . "' type='submit' name='" . $name . "' value=\"&nbsp;&nbsp;&nbsp;" . $value1 . "&nbsp;&nbsp;&nbsp;\" onclick=\"" . $jscript . "();\">";
	else
		echo "<input id='" . $name . "' type='submit' name='" . $name . "' value=\"&nbsp;&nbsp;&nbsp;" . $value1 . "&nbsp;&nbsp;&nbsp;\">";
	
	echo "</td>";
}

function FormButton($value1, $jscript = null) {
	echo "<tr height='30' valign='bottom'><td colspan='2' align='center'>";
	if ($jscript && $jscript != "")
		echo "<input id='button' type='submit' name='button1' value=\"&nbsp;&nbsp;&nbsp;" . $value1 . "&nbsp;&nbsp;&nbsp;\" onclick=\"return " . $jscript . "();\" onMouseOver=\"window.status=''; return true;\">";
	else
		echo "<input class='button' type='submit' name='button1' value=\"&nbsp;&nbsp;&nbsp;" . $value1 . "&nbsp;&nbsp;&nbsp;\" onMouseOver=\"window.status=''; return true;\">";
	
	echo "</td></tr>";
}

function FormButton2($value1, $value2, $jscript1 = "", $jscript2 = "") {
	echo "<tr height='30' valign='bottom'><td colspan='2' align='center'>";
	if ($jscript1 && $jscript1 != "")
		echo "<input class='button' type='submit' name='button1' value=\"&nbsp;&nbsp;&nbsp;" . $value1 . "&nbsp;&nbsp;&nbsp;\" onclick=\"return " . $jscript1 . "();\"  onMouseOver=\"window.status=''; return true;\">";
	else
		echo "<input class='button' type='submit' name='button1' value=\"&nbsp;&nbsp;&nbsp;" . $value1 . "&nbsp;&nbsp;&nbsp;\" onMouseOver=\"window.status=''; return true;\">";
	
	echo "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	
	if ($jscript2 && $jscript2 != "")
		echo "<input class='button' type='submit' name='button2' value=\"&nbsp;&nbsp;&nbsp;" . $value2 . "&nbsp;&nbsp;&nbsp;\" onclick=\"return " . $jscript2 . "();\"  onMouseOver=\"window.status=''; return true;\">";
	else
		echo "<input class='button' type='submit' name='button2' value=\"&nbsp;&nbsp;&nbsp;" . $value2 . "&nbsp;&nbsp;&nbsp;\" onMouseOver=\"window.status=''; return true;\">";
	
	echo "</td></tr>";
}

function InitList($columns, $id=null, $class="", $align="") {
	global $listColumns, $listCurrent, $listRow, $listwidth;
	
	$listColumns = $columns;
	$listCurrent = 1;
	$listRow = 0;
	$listwidth = 2;
	
	echo "<table id='$id' class='list-table $class' cellspacing='1' cellpadding='1' align='$align'>";
    print "<thead>";
    echo "<tr height='20'>";
}

function ListHeader($title, $width = "", $url = "") {
	global $listColumns, $listCurrent, $listwidth;

	if($width != "") {
        $listwidth += ($width +2);
    } else {
        $width = "auto";
    }

	if($url) {
		echo "<th width='$width' bgcolor='#334477' nowrap><a href='$url'><font color='white' size='1' face='Verdana'>&nbsp;" . $title . "</font></th>";
    } else {
		echo "<th width='$width' bgcolor='#334477' nowrap><font color='white' size='1' face='Verdana'>&nbsp;" . $title . "</font></th>";
    }
    $listCurrent++;
	if($listCurrent > $listColumns) {
		$listCurrent = 1;
		echo "</tr>";
        print "</thead>";
        print "<tbody>";
	}
}

function TotalItem($title, $align) {
	global $listColumns, $listCurrent, $listRow;
	
	if ($listCurrent == 1) {
		$listRow++;
		$bgcolor = "#BBBBBB";
		echo "<tr bgcolor='$bgcolor' height='20'>";
	}
	
	echo "<td align='" . $align . "'><font size='1' face='Verdana'><b>" . $title . "</b></font></td>";
	
	$listCurrent++;
	if ($listCurrent > $listColumns) {
		$listCurrent = 1;
		echo "</tr>";
	}
}

function ListItem($title, $align, $url) {
	global $listColumns, $listCurrent, $listRow;
	
	if ($listCurrent == 1) {
		$listRow++;
		if ($listRow == 1)
			$bgcolor = "#FFFFFF";
		else {
			$listRow = 0;
			$bgcolor = "#DDDDDD";
		}
		echo "<tr bgcolor='".$bgcolor."' text='#000000' height='20' >";
	}
	
	if($url)
		echo "<td align='".$align."' style='vertical-align:middle'><p class='litem'><a href='index.php?formcode=" . $url . "'>".$title."</a></p></td>";
	else
		echo "<td align='".$align."' style='vertical-align:middle'><font size='1' face='Verdana'>".$title."</font></td>";
	
	$listCurrent++;
	if ($listCurrent > $listColumns) {
		$listCurrent = 1;
		echo "</tr>";
	}
}

function ListItemChat($title, $align) {
	global $listColumns, $listCurrent, $listRow;
	
	if ($listCurrent == 1) {
		$listRow++;
		if ($listRow == 1)
			$bgcolor = "#FFFFFF";
		else {
			$listRow = 0;
			$bgcolor = "#DDDDDD";
		}
		echo "<tr bgcolor='" . $bgcolor . "' text='#000000' height='20' >";
	}
	
	echo "<td align='" . $align . "'><font size='1' face='Verdana' color='#0000ff'>" . $title . "</font></td>";
	
	$listCurrent++;
	if ($listCurrent > $listColumns) {
		$listCurrent = 1;
		echo "</tr>";
	}
}

function ListItem1($title, $align, $url, $param1) {
	global $listColumns, $listCurrent, $listRow;
	
	if ($listCurrent == 1) {
		$listRow++;
		if ($listRow == 1)
			$bgcolor = "#FFFFFF";
		else {
			$listRow = 0;
			$bgcolor = "#DDDDDD";
		}
		echo "<tr bgcolor='" . $bgcolor . "' text='#000000' height='20' >";
	}
	
	if ($url)
		echo "<td align='" . $align . "'><p class='litem'><a href='index.php?formcode=" . $url . "&param1=" . $param1 . "'>" . $title . "</a></p></td>";
	else
		echo "<td align='" . $align . "'><font size='1' face='Verdana'>" . $title . "</font></td>";
	
	$listCurrent++;
	if ($listCurrent > $listColumns) {
		$listCurrent = 1;
		echo "</tr>";
	}
}

function ListItem2($title, $align, $url, $param1, $param2) {
	global $listColumns, $listCurrent, $listRow;
	
	if ($listCurrent == 1) {
		$listRow++;
		if ($listRow == 1)
			$bgcolor = "#FFFFFF";
		else {
			$listRow = 0;
			$bgcolor = "#DDDDDD";
		}
		echo "<tr bgcolor='" . $bgcolor . "' text='#000000' height='20' >";
	}
	
	if ($url)
		echo "<td align='" . $align . "'><p class='litem'><a href='index.php?formcode=" . $url . "&param1=" . $param1 . "&param2=" . $param2 . "'>" . $title . "</a></p></td>";
	else
		echo "<td align='" . $align . "'><font size='1' face='Verdana'>" . $title . "</font></td>";
	
	$listCurrent++;
	if ($listCurrent > $listColumns) {
		$listCurrent = 1;
		echo "</tr>";
	}
}

function ListItem2JS($title, $align, $url, $param1, $param2, $jscript, $text) {
	global $listColumns, $listCurrent, $listRow;
	
	if ($listCurrent == 1) {
		$listRow++;
		if ($listRow == 1)
			$bgcolor = "#FFFFFF";
		else {
			$listRow = 0;
			$bgcolor = "#DDDDDD";
		}
		echo "<tr bgcolor='" . $bgcolor . "' text='#000000' height='20' >";
	}
	if ($jscript) {
		if ($url)
			echo "<td align='" . $align . "'><p class='litem'><a href='index.php?formcode=" . $url . "&param1=" . $param1 . "&param2=" . $param2 . "' onclick='return " . $jscript . "(" . $text . ");'>" . $title . "</a></p></td>";
		else
			echo "<td align='" . $align . "'><font size='1' face='Verdana'><a href='#' onclick='javascript:" . $jscript . "(" . $text . ");'>" . $title . "</a></font></td>";
	}
	else
	{
		if ($url)
			echo "<td align='" . $align . "'><p class='litem'><a href='index.php?formcode=" . $url . "&param1=" . $param1 . "&param2=" . $param2 . "' >" . $title . "</a></p></td>";
		else
			echo "<td align='" . $align . "'><font size='1' face='Verdana'>" . $title . "</font></td>";
	}
	$listCurrent++;
	if ($listCurrent > $listColumns) {
		$listCurrent = 1;
		echo "</tr>";
	}
}

function ListItem2popup($title, $align, $url, $param1, $param2=false) {
	global $listColumns, $listCurrent, $listRow;
	
	if ($listCurrent == 1) {
		$listRow++;
		if ($listRow == 1)
			$bgcolor = "#FFFFFF";
		else {
			$listRow = 0;
			$bgcolor = "#DDDDDD";
		}
		echo "<tr bgcolor='" . $bgcolor . "' text='#000000' height='20' >";
	}
	
	echo "<td align='" . $align . "'><p class='litem'>";
	//echo "<a href='".$url."?param1=".$param1."&param2=".$param2."' target='_blank' ";
	//echo "<a href=\"javascript:window.open('".$url."?param1=".$param1."&param2=".$param2."','Show Hand ".$param1."', 'width=600,height=700')\">";
	echo "<a href='#' onClick=\"window.open('" . $url . "?param1=" . $param1;
    if($param2){
        echo "&param2=".$param2;
    }
    echo "','hand" . $param1 . "','width=600,height=700,menubar=0,scrollbars=1,resizable=1'); return false;\">";
	echo $title . "</a></p></td>";
	
	$listCurrent++;
	if ($listCurrent > $listColumns) {
		$listCurrent = 1;
		echo "</tr>";
	}
}

function ListItem3($title, $align, $url, $param1, $param2, $param3) {
	global $listColumns, $listCurrent, $listRow;
	
	if ($listCurrent == 1) {
		$listRow++;
		if ($listRow == 1)
			$bgcolor = "#FFFFFF";
		else {
			$listRow = 0;
			$bgcolor = "#DDDDDD";
		}
		echo "<tr bgcolor='" . $bgcolor . "' text='#000000' height='20' >";
	}
	
	if ($url)
		echo "<td align='" . $align . "'><p class='litem'><a href='index.php?formcode=" . $url . "&param1=" . $param1 . "&param2=" . $param2 . "&param3=" . $param3 . "'>" . $title . "</a></p></td>";
	else
		echo "<td align='" . $align . "'><font size='1' face='Verdana'>" . $title . "</font></td>";
	
	$listCurrent++;
	if ($listCurrent > $listColumns) {
		$listCurrent = 1;
		echo "</tr>";
	}
}

function ListImage($title, $align, $url) {
	global $listColumns, $listCurrent, $listRow;
	
	if ($listCurrent == 1) {
		$listRow++;
		if ($listRow == 1)
			$bgcolor = "#FFFFFF";
		else {
			$listRow = 0;
			$bgcolor = "#DDDDDD";
		}
		echo "<tr bgcolor='" . $bgcolor . "' text='#000000' height='20' >";
	}
	
	if ($url)
		echo "<td align='" . $align . "'><a href='index.php?m=" . $url . "'><img border='0' src='" . $title . "'></a></td>";
	else
		echo "<td align='" . $align . "'><img border='0' src='" . $title . "'></td>";
	
	$listCurrent++;
	if ($listCurrent > $listColumns) {
		$listCurrent = 1;
		echo "</tr>";
	}
}

function ListImage1($title, $align, $url, $param1, $lparam = "param1", $alt = null) {
	global $listColumns, $listCurrent, $listRow;
	
	if ($listCurrent == 1) {
		$listRow++;
		if ($listRow == 1)
			$bgcolor = "#FFFFFF";
		else {
			$listRow = 0;
			$bgcolor = "#DDDDDD";
		}
		echo "<tr bgcolor='$bgcolor' text='#000000' height='20' >";
	}

    if(!$alt || $alt == null) {
        $alt = $param1;
    }

	if ($url)
		echo "<td align='$align'>
		        <a href='index.php?m=" . $url . "&" . $lparam . "=" . $param1 . "'>
		            <img border='0' src=\"$title\" alt=\"$alt\" /></a></td>";
	else
		echo "<td align='$align'>
		        <img border='0' src='" . $title . "' /></td>";
	
	$listCurrent++;
	if ($listCurrent > $listColumns) {
		$listCurrent = 1;
		echo "</tr>";
	}
}

function ListImageJs_rev($imagepath, $align="center", $alt="", $ahref="javascript:void(0);", $extra="") {
    global $listColumns, $listCurrent, $listRow;

    if ($listCurrent == 1) {
        $listRow++;
        if ($listRow == 1)
            $bgcolor = "#FFFFFF";
        else {
            $listRow = 0;
            $bgcolor = "#DDDDDD";
        }
        echo "<tr bgcolor='" . $bgcolor . "' text='#000000' height='20' >";
    }

    echo "<td align='$align'>
            <a href='$ahref' $extra >
                <img border='0' src='$imagepath' alt='$alt' />
            </a>
          </td>";


    $listCurrent++;
    if ($listCurrent > $listColumns) {
        $listCurrent = 1;
        echo "</tr>";
    }

}


function ListImageJS($title, $align, $jscript, $event="onClick", $alt = "") {
	global $listColumns, $listCurrent, $listRow;
	
	if ($listCurrent == 1) {
		$listRow++;
		if ($listRow == 1)
			$bgcolor = "#FFFFFF";
		else {
			$listRow = 0;
			$bgcolor = "#DDDDDD";
		}
		echo "<tr bgcolor='" . $bgcolor . "' text='#000000' height='20' >";
	}
	
	if ($jscript) {
		echo "<td align='$align'>
		        <a href=\"javascript:void(0);\" ".$event."=\"javascript:".$jscript.";\">
		            <img border='0' src=\"$title\" alt='$alt' /></a></td>";
    } else {
		echo "<td align='$align'>
		        <img border='0' src=\"$title\" alt='$alt' /></td>";
    }

	$listCurrent++;
	if ($listCurrent > $listColumns) {
		$listCurrent = 1;
		echo "</tr>";
	}
}

function ListImage2($title, $align, $url, $param1, $param2) {
	global $listColumns, $listCurrent, $listRow;
	if ($listCurrent == 1) {
		$listRow++;
		if ($listRow == 1)
			$bgcolor = "#FFFFFF";
		else {
			$listRow = 0;
			$bgcolor = "#DDDDDD";
		}
		echo "<tr bgcolor='" . $bgcolor . "' text='#000000' height='20' >";
	}
	
	if ($url)
		echo "<td align='" . $align . "'><a href='index.php?m=" . $url . "&param1=" . $param1 . "&param2=" . $param2 . "'><img border='0' src='" . $title . "'></a></td>";
	else
		echo "<td align='" . $align . "'><img border='0' src='" . $title . "'></td>";
	
	$listCurrent++;
	if ($listCurrent > $listColumns) {
		$listCurrent = 1;
		echo "</tr>";
	}
}

$table_columns = 0;
$table_curcol = 0;
function StartTable($border, $columns) {
	global $table_columns, $table_curcol;
	
	$table_columns = $columns;
	$table_curcol = 0;
	
	echo "<table border='" . $border . "' cellspacing='5' cellpadding='2'>";
    print "<thead>";
}

function TableHeader($title) {
	global $table_columns;

	echo "<tr height='20'><th colspan='" . $table_columns . "' bgcolor='#003399'><font color='white'><b>&nbsp;" . $title . "</b></font></th></tr></thead>\n";
}

function UserListDisplay($locarray, $initcol, $current_page = 1, $row_per_page = 5) {

	InitList(7);
	ListHeader("", 20);
	ListHeader("User Name", 120, "index.php?sortcol=1");
	ListHeader("Login Date", 180, "index.php?sortcol=2");
	ListHeader("Currency", 120, "index.php?sortcol=3");
	ListHeader("Balance Play", 100, "index.php?sortcol=4");
	ListHeader("Brand", 120, "index.php?sortcol=5");
	ListHeader("Admin Level", 120, "index.php?sortcol=6");
	
	if ($initcol == -1) {
		// init part
		$_SESSION["locarray"] = $locarray;
		$_SESSION["ascending"] = false;
		$_SESSION["search_php"] = "index.php";
		$_SESSION["totrow"] = count($_SESSION["locarray"]);
		$_SESSION["search_totpage"] = ceil($_SESSION["totrow"] / $row_per_page); // number of pages 
		$_SESSION["current_page"] = 1;
		$_SESSION["sortcol"] = 1;
		
		$start_showing = 1;
		$end_showing = ($row_per_page);
		if ($_SESSION["totrow"] < $row_per_page)
			$end_showing = $_SESSION["totrow"];
		
		for ($i = $start_showing; $i <= $end_showing; $i++) {
			$row = $locarray[$i -1]; // Arrays start from 0 
			ListImage1("icon/edit.png", "center", "index", $row[0]);
			ListItem($row[1], "left", "");
			//ListItem( $row[2], "left", "");
			ListItem($row[2], "center", "");
			ListItem($row[3], "center", "");
			ListItem(number_format($row[4] / 100, 2), "center", "");
			ListItem($row[5], "left", "");
			ListItem($row[6], "left", "");
		}
	} else
		if (isset ($_REQUEST["sortcol"])) {
			//        	  echo "Did I get here first?";  
			$sarray = array ();
			//$locarray = $result;
			foreach ($locarray as $row) {
				array_push($sarray, $row[$initcol]);
			}
			//      So every time a user clicks on a column header she/he gets an ascending or descending order 
			if ($_SESSION["ascending"]) {
				natcasesort($sarray);
				$_SESSION["ascending"] = false;
			} else {
				
				natcasesort($sarray);
				$tsarray = array ();
				foreach ($sarray as $key => $val) {
					$tsarray = array (
						"$key" => "$val"
					) + $tsarray;
				}
				
				$sarray = $tsarray;
				//                  echo $tsarray;
				
				$_SESSION["ascending"] = true;
			}
			$temparray = array ();
			foreach ($sarray as $key => $val) {
				$row = $locarray[$key];
				array_push($temparray, $row);
			}
			
			$_SESSION["locarray"] = $temparray;
			
			$start_showing = ($row_per_page * ($current_page -1)) + 1;
			$end_showing = ($row_per_page * ($current_page));
			
			if ($_SESSION["totrow"] < $end_showing)
				$end_showing = $_SESSION["totrow"];
			
			//             echo "--------------startshow=".$start_showing;
			//             echo "-------------endshow=".$end_showing;
			
			for ($i = $start_showing; $i <= $end_showing; $i++) {
				$row = $temparray[$i -1];
				ListImage1("icon/edit.png", "center", "index", $row[0]);
				ListItem($row[1], "left", "");
				//ListItem( $row[2], "left", "");
				ListItem($row[2], "center", "");
				ListItem($row[3], "center", "");
				ListItem(number_format($row[4] / 100, 2), "center", "");
				ListItem($row[5], "left", "");
				ListItem($row[6], "left", "");
			}
		} else {
			$start_showing = ($row_per_page * ($current_page -1)) + 1;
			$end_showing = ($row_per_page * ($current_page));
			
			if ($_SESSION["totrow"] < $end_showing)
				$end_showing = $_SESSION["totrow"];
			
			// echo "--------------startshow=".$start_showing;
			// echo "-------------endshow=".$end_showing;
			
			for ($i = $start_showing; $i <= $end_showing; $i++) {
				$row = $locarray[$i -1];
				ListImage1("icon/edit.png", "center", "index", $row[0]);
				ListItem($row[1], "left", "");
				//ListItem( $row[2], "left", "");
				ListItem($row[2], "center", "");
				ListItem($row[3], "center", "");
				ListItem(number_format($row[4] / 100, 2), "center", "");
				ListItem($row[5], "left", "");
				ListItem($row[6], "left", "");
			}
		}
	
	EndList();
	page_object($current_page);
	
}

function PlayerListDisplay($locarray, $initcol, $current_page = 1, $row_per_page = 5) {
	
	InitList(8);
	ListHeader("", 20);
	ListHeader("User Name", 120, "index.php?sortplaycol=1");
	ListHeader("Display Name", 180, "index.php?sortplaycol=2");
	ListHeader("Country", 120, "index.php?sortplaycol=3");
	ListHeader("Date Created", 180, "index.php?sortplaycol=4");
	ListHeader("Login Date", 180, "index.php?sortplaycol=5");
	ListHeader("Balance Money", 100, "index.php?sortplaycol=6");
	ListHeader("Brand", 120, "index.php?sortplaycol=7");
	
	if ($initcol == -1) {
		// init part
		$_SESSION["locarray"] = $locarray;
		$_SESSION["ascending"] = false;
		$_SESSION["search_php"] = "index.php";
		$_SESSION["totrow"] = count($_SESSION["locarray"]);
		$_SESSION["search_totpage"] = ceil($_SESSION["totrow"] / $row_per_page); // number of pages 
		$_SESSION["current_page"] = 1;
		$_SESSION["sortplaycol"] = 1;
		
		$start_showing = 1;
		$end_showing = ($row_per_page);
		if ($_SESSION["totrow"] < $row_per_page)
			$end_showing = $_SESSION["totrow"];
		
		for ($i = $start_showing; $i <= $end_showing; $i++) {
			$row = $locarray[$i -1]; // Arrays start from 0 
			ListImage1("icon/edit.png", "center", "index", $row[0], "playparam1");
			ListItem($row[1], "left", "");
			//ListItem( $row[2], "left", "");
			ListItem($row[2], "center", "");
			ListItem($row[3], "center", "");
			ListItem($row[4], "center", "");
			ListItem($row[5], "left", "");
			ListItem(number_format($row[6] / 100, 2), "center", "");
			ListItem($row[7], "left", "");
		}
	} else
		if (isset ($_REQUEST["sortplaycol"])) {
			// echo "Did I get here first?";  
			$sarray = array ();
			//$locarray = $result;
			foreach ($locarray as $row) {
				array_push($sarray, $row[$initcol]);
			}
			//      So every time a user clicks on a column header she/he gets an ascending or descending order 
			if ($_SESSION["ascending"]) {
				natcasesort($sarray);
				$_SESSION["ascending"] = false;
			} else {
				
				natcasesort($sarray);
				$tsarray = array ();
				foreach ($sarray as $key => $val) {
					$tsarray = array (
						"$key" => "$val"
					) + $tsarray;
				}
				
				$sarray = $tsarray;
				//                  echo $tsarray;
				
				$_SESSION["ascending"] = true;
			}
			$temparray = array ();
			foreach ($sarray as $key => $val) {
				$row = $locarray[$key];
				array_push($temparray, $row);
			}
			
			$_SESSION["locarray"] = $temparray;
			
			$start_showing = ($row_per_page * ($current_page -1)) + 1;
			$end_showing = ($row_per_page * ($current_page));
			
			if ($_SESSION["totrow"] < $end_showing)
				$end_showing = $_SESSION["totrow"];
			
			//             echo "--------------startshow=".$start_showing;
			//             echo "-------------endshow=".$end_showing;
			
			for ($i = $start_showing; $i <= $end_showing; $i++) {
				$row = $temparray[$i -1];
				ListImage1("icon/edit.png", "center", "index", $row[0], "playparam1");
				ListItem($row[1], "left", "");
				//ListItem( $row[2], "left", "");
				ListItem($row[2], "center", "");
				ListItem($row[3], "center", "");
				ListItem($row[4], "center", "");
				ListItem($row[5], "left", "");
				ListItem(number_format($row[6] / 100, 2), "center", "");
				ListItem($row[7], "left", "");
			}
		} else {
			$start_showing = ($row_per_page * ($current_page -1)) + 1;
			$end_showing = ($row_per_page * ($current_page));
			
			if ($_SESSION["totrow"] < $end_showing)
				$end_showing = $_SESSION["totrow"];
			
			//             echo "--------------startshow=".$start_showing;
			//             echo "-------------endshow=".$end_showing;
			
			for ($i = $start_showing; $i <= $end_showing; $i++) {
				$row = $locarray[$i -1];
				ListImage1("icon/edit.png", "center", "index", $row[0], "playparam1");
				ListItem($row[1], "left", "");
				//ListItem( $row[2], "left", "");
				ListItem($row[2], "center", "");
				ListItem($row[3], "center", "");
				ListItem($row[4], "center", "");
				ListItem($row[5], "left", "");
				ListItem(number_format($row[6] / 100, 2), "center", "");
				ListItem($row[7], "left", "");
			}
		}
	
	EndList();
	page_object();
	
}

function EndList() {
    print "</tbody>";
    echo "</table>";
}

function page_object_old($curpage, $lpage = "page") {
	if ($_SESSION["search_totpage"] < 2)
		return;
	
	$totpage = $_SESSION["search_totpage"];
	$url = $_SESSION["search_php"];
	
	echo "<table cellspacing='0' bordercolordark='white' bordercolorlight='black' height='16' border='1' bgcolor='#B2B2B2'><tr>";
	echo "<td width='20'><a href='" . $url . "?" . $lpage . "=1'><img src='images/arrow1.gif' width='20' height='20' border='0'></a></td>";
	
	if ($curpage > 1)
		echo "<td width='20'><a href='" . $url . "?" . $lpage . "=" . ($curpage -1) . "'><img src='images/arrow2.gif' width='20' height='20' border='0'></a></td>";
	
	echo "<td width='150' align='center'><font size='2' face='Verdana'>Page " . $curpage . " of " . $totpage . "</font></td>";
	
	if ($curpage < $totpage)
		echo "<td width='20'><a href='" . $url . "?" . $lpage . "=" . ($curpage +1) . "'><img src='images/arrow3.gif' width='20' height='20' border='0'></a></td>";
	
	echo "<td width='20'><a href='" . $url . "?" . $lpage . "=" . $totpage . "'><img src='images/arrow4.gif' width='20' height='20' border='0'></a></td></tr></table>";
}

function page_object() {
	global $lang, $listwidth;
	//$pager_maxrows = 2;

	if ((isset($_SESSION["search_totrows"]) && $_SESSION["search_totrows"] < 1) || func_num_args() < 1) {
        return;
    }/*
    else {

        if(func_num_args() > 0) {
            foreach(func_get_args() as $key => $arg) {
                if(strstr(strtolower($arg), "pager")) {
                    list($p, $v) = explode("=", $arg);
                    $pager_maxrows = (int)$arg;
                }
                print "$key => $arg<br/>";
            }
        }*/
        if($_SESSION["search_totpage"] < 2) {
			//echo "<table cellspacing='0' bordercolordark='white' bordercolorlight='black' height='16' border='1' bgcolor='#B2B2B2' width='" . $listwidth . "'><tr>";
			//echo "<tr><td bgcolor='#FFFFDD'>&nbsp;"./*sprintf($lang["rec_found"], $_SESSION["search_totrows"]).*/"</td></tr></table>";
            return;
		}
    //}

	$curpage = func_get_arg(0);
	
	$extra = "";
	for($i = 1; $i < func_num_args(); $i++) {
		$field = func_get_arg($i);
		$extra .= "&" . $field . "=" . (isset($_REQUEST[$field]) ? $_REQUEST[$field] : "");
	}
	
	$totpage = $_SESSION["search_totpage"];
	$url = "index.php?m=".$_SESSION["search_php"];
    
	echo "<table cellspacing='0' cellpadding='2' border='0' width='100%' align='center'>";
	echo "<tr>";
    echo "<td>&nbsp;</td>";
    echo "<td width='20'><a href='" . $url . "&page=1" . $extra . "'><img src='images/arrow1.gif' width='20' height='20' border='0'></a></td>";
	
	if($curpage > 1)
		echo "<td width='20'><a href='" . $url . "&page=" . ($curpage -1) . $extra . "'><img src='images/arrow2.gif' width='20' height='20' border='0'></a></td>";
	
	echo "<td width='120' align='center'><b>Page " . $curpage . " of " . $totpage . "</b></td>";
	
	if ($curpage < $totpage) {
		//$columns--;
		echo "<td width='20'><a href='" . $url . "&page=" . ($curpage +1) . $extra . "'><img src='images/arrow3.gif' width='20' height='20' border='0'></a></td>";
	}
	
	echo "<td width='20'><a href='" . $url . "&page=" . $totpage . $extra . "'><img src='images/arrow4.gif' width='20' height='20' border='0'></a></td>";
	
	echo "<td>&nbsp;"./* sprintf($lang["rec_found"], $_SESSION["search_totrows"]) .*/"</td>";
	echo "</tr></table><br/>";
}

function FormFileUpload($label, $name="uploadedfile") {
	echo "<tr><td bgcolor='#555555' style='color:#ffffff'>";
    echo "&nbsp;$label&nbsp;&nbsp;&nbsp;";
	echo "</td>";
    echo "<td bgcolor='#999999'>&nbsp;";
	echo "<form enctype='multipart/form-data' action='index.php' method='POST'>";
	echo "<input type='hidden' name='MAX_FILE_SIZE' value='100000' />";
	echo "<input type='hidden' name='formcode' value='player_upload' />";
	echo "<!-- Choose a file to upload: --> <input name='$name' type='file' />";
	echo "<input type='submit' value='Upload File' /></form>";
	echo "&nbsp;</td></tr>";
}

function FormUploadSingle($form_name="upload_form", $field_name="uploaded_file", $action="index.php?m=msg_file_upload.php", $method="POST", $maxsize="100000") {
    echo "<form enctype='multipart/form-data' name='$form_name' action='$action' method='$method'>";
    echo "<input type='hidden' name='MAX_FILE_SIZE' value='$maxsize' />";
    echo "<input name='$field_name' type='file' />";
    echo "<input type='submit' value='Upload File' />";
    echo "</form>";
}

function RevenueListDisplay($locarray, $initcol, $current_page = 1, $row_per_page = 5) {

	InitList(12);
	ListHeader("", 20);
	ListHeader("", 20);
	ListHeader("Description", 180, "index.php?sortrevcol=2");
	ListHeader("Revenue Type", 100, "index.php?sortrevcol=3");
	ListHeader("Level Count", 80, "index.php?sortrevcol=4");
	ListHeader("Perc 1", 60, "index.php?sortrevcol=5");
	ListHeader("Level 2", 90, "index.php?sortrevcol=6");
	ListHeader("Perc 2", 60, "index.php?sortrevcol=7");
	ListHeader("Level 3", 90, "index.php?sortrevcol=8");
	ListHeader("Perc 3", 60, "index.php?sortrevcol=9");
	ListHeader("Level 4", 90, "index.php?sortrevcol=10");
	ListHeader("Perc 4", 60, "index.php?sortrevcol=11");
	
	if ($initcol == -1) {
		// init part
		$_SESSION["locarray"] = $locarray;
		$_SESSION["ascending"] = false;
		$_SESSION["search_php"] = "index.php";
		$_SESSION["totrow"] = count($_SESSION["locarray"]);
		$_SESSION["search_totpage"] = ceil($_SESSION["totrow"] / $row_per_page); // number of pages 
		$_SESSION["current_page"] = 1;
		$_SESSION["sortrevcol"] = 1;
		
		$start_showing = 1;
		$end_showing = ($row_per_page);
		if ($_SESSION["totrow"] < $row_per_page)
			$end_showing = $_SESSION["totrow"];
		
		for ($i = $start_showing; $i <= $end_showing; $i++) {
			$row = $locarray[$i -1]; // Arrays start from 0 
			
			ListImage1("icon/edit.png", "center", "index", $row[0], "revparam1");
			if ($_SESSION["muser_id"] == $row[1])
				ListImage1("icon/delete.png", "center", "revenue_delete", $row[0], "revparam1");
			else
				ListItem("", "left", "");
			
			ListItem($row[3], "left", "");
			
			switch ($row[4]) {
				case 1 :
					ListItem("Player Count", "center", "");
					break;
				case 2 :
					ListItem("Rake Amount", "center", "");
					break;
				default :
					ListItem("Unknown", "center", "");
				break;
			}
			
			ListItem($row[5], "center", "");
			ListItem($row[7] . "%", "center", "");
			if ($row[5] > 1) {
				ListItem($row[8], "center", "");
				ListItem($row[9] . "%", "center", "");
			} else {
				ListItem("-", "center", "");
				ListItem("-", "center", "");
			}
			if ($row[5] > 2) {
				ListItem($row[10], "center", "");
				ListItem($row[11] . "%", "center", "");
			} else {
				ListItem("-", "center", "");
				ListItem("-", "center", "");
			}
			if ($row[5] > 3) {
				ListItem($row[12], "center", "");
				ListItem($row[13] . "%", "center", "");
			} else {
				ListItem("-", "center", "");
				ListItem("-", "center", "");
			}
			
		}
	} else
		if (isset ($_REQUEST["sortrevcol"])) {
			//        	  echo "Did I get here first?";  
			$sarray = array ();
			//$locarray = $result;
			foreach ($locarray as $row) {
				array_push($sarray, $row[$initcol]);
			}
			//      So every time a user clicks on a column header she/he gets an ascending or descending order 
			if ($_SESSION["ascending"]) {
				natcasesort($sarray);
				$_SESSION["ascending"] = false;
			} else {
				
				natcasesort($sarray);
				$tsarray = array ();
				foreach ($sarray as $key => $val) {
					$tsarray = array (
						"$key" => "$val"
					) + $tsarray;
				}
				
				$sarray = $tsarray;
				//                  echo $tsarray;
				
				$_SESSION["ascending"] = true;
			}
			$temparray = array ();
			foreach ($sarray as $key => $val) {
				$row = $locarray[$key];
				array_push($temparray, $row);
			}
			
			$_SESSION["locarray"] = $temparray;
			
			$start_showing = ($row_per_page * ($current_page -1)) + 1;
			$end_showing = ($row_per_page * ($current_page));
			
			if ($_SESSION["totrow"] < $end_showing)
				$end_showing = $_SESSION["totrow"];
			
			//echo "--------------startshow=".$start_showing;
			//echo "-------------endshow=".$end_showing;
			
			for ($i = $start_showing; $i <= $end_showing; $i++) {
				$row = $temparray[$i -1];
				
				ListImage1("icon/edit.png", "center", "index", $row[0], "revparam1");
				if ($_SESSION["muser_id"] == $row[1])
					ListImage1("icon/delete.png", "center", "revenue_delete", $row[0], "revparam1");
				else
					ListItem("", "left", "");
				
				ListItem($row[3], "left", "");
				
				switch ($row[4]) {
					case 1 :
						ListItem("Player Count", "center", "");
						break;
					case 2 :
						ListItem("Rake Amount", "center", "");
						break;
					default :
						ListItem("Unknown", "center", "");
					break;
				}
				
				ListItem($row[5], "center", "");
				ListItem($row[7] . "%", "center", "");
				if ($row[5] > 1) {
					ListItem($row[8], "center", "");
					ListItem($row[9] . "%", "center", "");
				} else {
					ListItem("-", "center", "");
					ListItem("-", "center", "");
				}
				if ($row[5] > 2) {
					ListItem($row[10], "center", "");
					ListItem($row[11] . "%", "center", "");
				} else {
					ListItem("-", "center", "");
					ListItem("-", "center", "");
				}
				if ($row[5] > 3) {
					ListItem($row[12], "center", "");
					ListItem($row[13] . "%", "center", "");
				} else {
					ListItem("-", "center", "");
					ListItem("-", "center", "");
				}
			}
		} else {
			$start_showing = ($row_per_page * ($current_page -1)) + 1;
			$end_showing = ($row_per_page * ($current_page));
			
			if ($_SESSION["totrow"] < $end_showing)
				$end_showing = $_SESSION["totrow"];
			
			//             echo "--------------startshow=".$start_showing;
			//             echo "-------------endshow=".$end_showing;
			
			for ($i = $start_showing; $i <= $end_showing; $i++) {
				$row = $locarray[$i -1];
				ListImage1("icon/edit.png", "center", "index", $row[0], "revparam1");
				if ($_SESSION["muser_id"] == $row[1])
					ListImage1("icon/delete.png", "center", "revenue_delete", $row[0], "revparam1");
				else
					ListItem("", "left", "");
				
				ListItem($row[3], "left", "");
				
				switch ($row[4]) {
					case 1 :
						ListItem("Player Count", "center", "");
						break;
					case 2 :
						ListItem("Rake Amount", "center", "");
						break;
					default :
						ListItem("Unknown", "center", "");
					break;
				}
				
				ListItem($row[5], "center", "");
				ListItem($row[7] . "%", "center", "");
				if ($row[5] > 1) {
					ListItem($row[8], "center", "");
					ListItem($row[9] . "%", "center", "");
				} else {
					ListItem("-", "center", "");
					ListItem("-", "center", "");
				}
				if ($row[5] > 2) {
					ListItem($row[10], "center", "");
					ListItem($row[11] . "%", "center", "");
				} else {
					ListItem("-", "center", "");
					ListItem("-", "center", "");
				}
				if ($row[5] > 3) {
					ListItem($row[12], "center", "");
					ListItem($row[13] . "%", "center", "");
				} else {
					ListItem("-", "center", "");
					ListItem("-", "center", "");
				}
			}
		}
	
	EndList();
	page_object($current_page, "revpage");
	
}

function BonusListDisplay($locarray, $initcol, $current_page = 1, $row_per_page = 5) {
	
	InitList(9);
	ListHeader("", 20);
	ListHeader("", 20); // It's for deleting a bonus      
	ListHeader("Bonus Code", 120, "index.php?sortboncol=1");
	ListHeader("Description", 180, "index.php?sortboncol=2");
	ListHeader("Type", 120, "index.php?sortboncol=3");
	ListHeader("Percentage", 180, "index.php?sortboncol=4");
	ListHeader("Max Amount", 180, "index.php?sortboncol=5");
	ListHeader("Expire (days)", 100, "index.php?sortboncol=6");
	ListHeader("Affiliate Name", 180, "index.php?sortboncol=7");
	
	if ($initcol == -1) {
		// init part
		$_SESSION["locarray"] = $locarray;
		$_SESSION["ascending"] = false;
		$_SESSION["search_php"] = "index.php";
		$_SESSION["totrow"] = count($_SESSION["locarray"]);
		$_SESSION["search_totpage"] = ceil($_SESSION["totrow"] / $row_per_page); // number of pages 
		$_SESSION["current_page"] = 1;
		$_SESSION["sortplaycol"] = 1;
		
		$start_showing = 1;
		$end_showing = ($row_per_page);
		if ($_SESSION["totrow"] < $row_per_page)
			$end_showing = $_SESSION["totrow"];
		
		for ($i = $start_showing; $i <= $end_showing; $i++) {
			$row = $locarray[$i -1]; // Arrays start from 0 
			ListImage1("icon/edit.png", "center", "index", $row[0], "bonparam1");
			ListImage1("icon/delete.png", "center", "delbonus", $row[0], "bonparam1");
			ListItem($row[1], "left", "");
			ListItem($row[2], "left", "");
			ListItem($row[3], "left", "");
			//		            switch ( $row[3] ) 
			//		            {
			//		            case 0:
			//		                ListItem( "Refill", "left", "");
			//		                break;
			//		            case 1:
			//		                ListItem( "Unique", "left", "");
			//		                break;
			//		            case 2:
			//		                ListItem( "First", "left", "");
			//		                break;
			//		            default:
			//		                ListItem( "Unknown", "left", "");
			//		                break;
			//		            }
			ListItem($row[4] . "%", "center", "");
			ListItem(number_format($row[5] / 100, 2), "center", "");
			ListItem($row[6], "center", "");
			ListItem($row[7], "center", "");
		}
	} else
		if (isset ($_REQUEST["sortboncol"])) {
			// echo "Did I get here first?";  
			$sarray = array ();
			//$locarray = $result;
			foreach ($locarray as $row) {
				array_push($sarray, $row[$initcol]);
			}
			//      So every time a user clicks on a column header she/he gets an ascending or descending order 
			if ($_SESSION["ascending"]) {
				natcasesort($sarray);
				$_SESSION["ascending"] = false;
			} else {
				natcasesort($sarray);
				$tsarray = array ();
				foreach ($sarray as $key => $val) {
					$tsarray = array (
						"$key" => "$val"
					) + $tsarray;
				}
				
				$sarray = $tsarray;
				//                  echo $tsarray;
				
				$_SESSION["ascending"] = true;
			}
			$temparray = array ();
			foreach ($sarray as $key => $val) {
				$row = $locarray[$key];
				array_push($temparray, $row);
			}
			
			$_SESSION["locarray"] = $temparray;
			
			$start_showing = ($row_per_page * ($current_page -1)) + 1;
			$end_showing = ($row_per_page * ($current_page));
			
			if ($_SESSION["totrow"] < $end_showing)
				$end_showing = $_SESSION["totrow"];
			
			//             echo "--------------startshow=".$start_showing;
			//             echo "-------------endshow=".$end_showing;
			
			for ($i = $start_showing; $i <= $end_showing; $i++) {
				$row = $temparray[$i -1];
				ListImage1("icon/edit.png", "center", "index", $row[0], "bonparam1");
				ListImage1("icon/delete.png", "center", "delbonus", $row[0], "bonparam1");
				ListItem($row[1], "left", "");
				ListItem($row[2], "left", "");
				ListItem($row[3], "left", "");
				//		            switch ( $row[3] ) 
				//		            {
				//		            case 0:
				//		                ListItem( "Refill", "left", "");
				//		                break;
				//		            case 1:
				//		                ListItem( "Unique", "left", "");
				//		                break;
				//		            case 2:
				//		                ListItem( "First", "left", "");
				//		                break;
				//		            default:
				//		                ListItem( "Unknown", "left", "");
				//		                break;
				//		            }
				ListItem($row[4] . "%", "center", "");
				ListItem(number_format($row[5] / 100, 2), "center", "");
				ListItem($row[6], "center", "");
				ListItem($row[7], "center", "");
			}
		} else {
			$start_showing = ($row_per_page * ($current_page -1)) + 1;
			$end_showing = ($row_per_page * ($current_page));
			
			if ($_SESSION["totrow"] < $end_showing)
				$end_showing = $_SESSION["totrow"];
			
			//             echo "--------------startshow=".$start_showing;
			//             echo "-------------endshow=".$end_showing;
			
			for ($i = $start_showing; $i <= $end_showing; $i++) {
				$row = $locarray[$i -1];
				ListImage1("icon/edit.png", "center", "index", $row[0], "bonparam1");
				ListImage1("icon/delete.png", "center", "delbonus", $row[0], "bonparam1");
				ListItem($row[1], "left", "");
				ListItem($row[2], "left", "");
				ListItem($row[3], "left", "");
				//		            switch ( $row[3] ) 
				//		            {
				//		            case 0:
				//		                ListItem( "Refill", "left", "");
				//		                break;
				//		            case 1:
				//		                ListItem( "Unique", "left", "");
				//		                break;
				//		            case 2:
				//		                ListItem( "First", "left", "");
				//		                break;
				//		            default:
				//		                ListItem( "Unknown", "left", "");
				//		                break;
				//		            }
				ListItem($row[4] . "%", "center", "");
				ListItem(number_format($row[5] / 100, 2), "center", "");
				ListItem($row[6], "center", "");
				ListItem($row[7], "center", "");
			}
		}
	
	EndList();
	page_object($current_page, "bonpage");
	
}

function BonusListDisplayAdmin($locarray, $initcol, $current_page = 1, $row_per_page = 5) {
	
	InitList(9);
	ListHeader("", 20);
	ListHeader("", 20); // It's for deleting a bonus      
	ListHeader("Bonus Description", 120, "index.php?sortbonadmcol=1");
	//	      ListHeader("Tournament ID", 180, "index.php?sortbonadmcol=2");
	ListHeader("Currency", 120, "index.php?sortbonadmcol=3");
	ListHeader("Minimum Deposit", 180, "index.php?sortbonadmcol=4");
	ListHeader("Maximum Deposit", 180, "index.php?sortbonadmcol=5");
	ListHeader("Percentage", 100, "index.php?sortbonadmcol=6");
	ListHeader("Conversion Points", 100, "index.php?sortbonadmcol=7");
	ListHeader("Bonus Type", 180, "index.php?sortbonadmcol=8");
	
	if ($initcol == -1) {
		// init part
		$_SESSION["locarray"] = $locarray;
		$_SESSION["ascending"] = false;
		$_SESSION["search_php"] = "index.php";
		$_SESSION["totrow"] = count($_SESSION["locarray"]);
		$_SESSION["search_totpage"] = ceil($_SESSION["totrow"] / $row_per_page); // number of pages 
		$_SESSION["current_page"] = 1;
		$_SESSION["sortplaycol"] = 1;
		
		$start_showing = 1;
		$end_showing = ($row_per_page);
		if ($_SESSION["totrow"] < $row_per_page)
			$end_showing = $_SESSION["totrow"];
		
		for ($i = $start_showing; $i <= $end_showing; $i++) {
			$row = $locarray[$i -1]; // Arrays start from 0 
			ListImage1("icon/edit.png", "center", "index", $row[0], "bonadmparam1");
			ListImage1("icon/delete.png", "center", "delbonus", $row[0], "bonadmparam1");
			ListItem($row[1], "left", "");
			//		            ListItem( $row[2], "left", "");
			ListItem($row[3], "left", "");
			ListItem($row[4], "left", "");
			ListItem($row[5], "left", "");
			ListItem($row[6] . "%", "center", "");
			ListItem($row[7], "center", "");
			ListItem($row[8], "center", "");
		}
	} else
		if (isset ($_REQUEST["sortbonadmcol"])) {
			// echo "Did I get here first?";  
			$sarray = array ();
			//$locarray = $result;
			foreach ($locarray as $row) {
				array_push($sarray, $row[$initcol]);
			}
			//      So every time a user clicks on a column header she/he gets an ascending or descending order 
			if ($_SESSION["ascending"]) {
				natcasesort($sarray);
				$_SESSION["ascending"] = false;
			} else {
				natcasesort($sarray);
				$tsarray = array ();
				foreach ($sarray as $key => $val) {
					$tsarray = array (
						"$key" => "$val"
					) + $tsarray;
				}
				
				$sarray = $tsarray;
				//echo $tsarray;
				
				$_SESSION["ascending"] = true;
			}
			$temparray = array ();
			foreach ($sarray as $key => $val) {
				$row = $locarray[$key];
				array_push($temparray, $row);
			}
			
			$_SESSION["locarray"] = $temparray;
			
			$start_showing = ($row_per_page * ($current_page -1)) + 1;
			$end_showing = ($row_per_page * ($current_page));
			
			if ($_SESSION["totrow"] < $end_showing)
				$end_showing = $_SESSION["totrow"];
			
			//             echo "--------------startshow=".$start_showing;
			//             echo "-------------endshow=".$end_showing;
			
			for ($i = $start_showing; $i <= $end_showing; $i++) {
				$row = $temparray[$i -1];
				ListImage1("icon/edit.png", "center", "index", $row[0], "bonadmparam1");
				ListImage1("icon/delete.png", "center", "delbonus", $row[0], "bonadmparam1");
				ListItem($row[1], "left", "");
				//		            ListItem( $row[2], "left", "");
				ListItem($row[3], "left", "");
				ListItem($row[4], "left", "");
				ListItem($row[5], "left", "");
				ListItem($row[6] . "%", "center", "");
				ListItem($row[7], "center", "");
				ListItem($row[8], "center", "");
			}
		} else {
			$start_showing = ($row_per_page * ($current_page -1)) + 1;
			$end_showing = ($row_per_page * ($current_page));
			
			if ($_SESSION["totrow"] < $end_showing)
				$end_showing = $_SESSION["totrow"];
			
			//             echo "--------------startshow=".$start_showing;
			//             echo "-------------endshow=".$end_showing;
			
			for ($i = $start_showing; $i <= $end_showing; $i++) {
				$row = $locarray[$i -1];
				ListImage1("icon/edit.png", "center", "index", $row[0], "bonadmparam1");
				ListImage1("icon/delete.png", "center", "delbonus", $row[0], "bonadmparam1");
				ListItem($row[1], "left", "");
				//		            ListItem( $row[2], "left", "");
				ListItem($row[3], "left", "");
				ListItem($row[4], "left", "");
				ListItem($row[5], "left", "");
				ListItem($row[6] . "%", "center", "");
				ListItem($row[7], "center", "");
				ListItem($row[8], "center", "");
			}
		}
	
	EndList();
	page_object($current_page, "bonadmpage");
	
}

function ShowListJurisdiction() {
	global $lang;
	
	$usrlevel = $_SESSION["muser_level"];
	
	if ($usrlevel < 12)
		TBodyStart("showmanager", "1");
	else
		TBodyStart("showmanager", "");
	
	if ($_SESSION["muser_manager_id"] > 0)
		$parent_id = FormListBoxDB($lang["level11"], "showmanager", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and manager_id=" . $_SESSION["muser_manager_id"] . " and authority=11 order by user_name", "SearchRegionUser");
	else
		$parent_id = FormListBoxDB($lang["level11"], "showmanager", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and authority=11 order by user_name", "SearchRegionUser", "- all managers -");
	
	TBodyEnd();
	
	if ($usrlevel < 13)
		TBodyStart("showregion", "1");
	else
		TBodyStart("showregion", "");
	
	if ($_SESSION["muser_region_id"] > 0)
		$parent_id = FormListBoxDB($lang["level12"], "showregion", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and region_id=" . $_SESSION["muser_region_id"] . " and authority=12 order by user_name", "SearchDistrictUser");
	else
		$parent_id = FormListBoxDB($lang["level12"], "showregion", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and manager_id=" . $parent_id . " and authority=12 order by user_name", "SearchDistrictUser", "- all regions -");
	
	TBodyEnd();
	
	if ($usrlevel < 14)
		TBodyStart("showdistrict", "1");
	else
		TBodyStart("showdistrict", "");
	
	if ($_SESSION["muser_district_id"] > 0)
		$parent_id = FormListBoxDB($lang["level13"], "showdistrict", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and district_id=" . $_SESSION["muser_district_id"] . " and authority=13 order by user_name", "SearchAffiliateUser");
	else
		$parent_id = FormListBoxDB($lang["level13"], "showdistrict", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and region_id=" . $parent_id . " and authority=13 order by user_name", "SearchAffiliateUser", "- all districts -");
	
	TBodyEnd();
	
	if ($usrlevel < 15)
		TBodyStart("showaffiliate", "1");
	else
		TBodyStart("showaffiliate", "");
	
	if ($_SESSION["muser_affiliate_id"] > 0)
		$parent_id = FormListBoxDB($lang["level14"], "showaffiliate", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and affiliate_id=" . $_SESSION["muser_affiliate_id"] . " and authority=14 order by user_name", "");
	else
		$parent_id = FormListBoxDB($lang["level14"], "showaffiliate", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and district_id=" . $parent_id . " and authority=14 order by user_name", "", "- all affiliates -");
	
	TBodyEnd();
	
}

function ShowListJurisdiction2($usrlevel) {
	global $lang;
	
	if ($usrlevel < 12)
		TBodyStart("showmanager", "1");
	else
		TBodyStart("showmanager", "");
	
	if ($_SESSION["muser_manager_id"] > 0)
		$parent_id = FormListBoxDB($lang["level11"], "showmanager", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and manager_id=" . $_SESSION["muser_manager_id"] . " and authority=11 order by user_name", "SearchRegionUser");
	else
		$parent_id = FormListBoxDB($lang["level11"], "showmanager", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and authority=11 order by user_name", "SearchRegionUser", "- all managers -");
	
	TBodyEnd();
	
	if ($usrlevel < 13)
		TBodyStart("showregion", "1");
	else
		TBodyStart("showregion", "");
	
	if ($_SESSION["muser_region_id"] > 0)
		$parent_id = FormListBoxDB($lang["level12"], "showregion", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and region_id=" . $_SESSION["muser_region_id"] . " and authority=12 order by user_name", "SearchDistrictUser");
	else
		$parent_id = FormListBoxDB($lang["level12"], "showregion", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and manager_id=" . $parent_id . " and authority=12 order by user_name", "SearchDistrictUser", "- all regions -");
	
	TBodyEnd();
	
	if ($usrlevel < 14)
		TBodyStart("showdistrict", "1");
	else
		TBodyStart("showdistrict", "");
	
	if ($_SESSION["muser_district_id"] > 0)
		$parent_id = FormListBoxDB($lang["level13"], "showdistrict", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and district_id=" . $_SESSION["muser_district_id"] . " and authority=13 order by user_name", "SearchAffiliateUser");
	else
		$parent_id = FormListBoxDB($lang["level13"], "showdistrict", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and region_id=" . $parent_id . " and authority=13 order by user_name", "SearchAffiliateUser", "- all districts -");
	
	TBodyEnd();
	
	if ($usrlevel < 15)
		TBodyStart("showaffiliate", "1");
	else
		TBodyStart("showaffiliate", "");
	
	if ($_SESSION["muser_affiliate_id"] > 0)
		$parent_id = FormListBoxDB($lang["level14"], "showaffiliate", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and affiliate_id=" . $_SESSION["muser_affiliate_id"] . " and authority=14 order by user_name", "");
	else
		$parent_id = FormListBoxDB($lang["level14"], "showaffiliate", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . " and district_id=" . $parent_id . " and authority=14 order by user_name", "", "- all affiliates -");
	
	TBodyEnd();
	
}

function ShowListJurisdictionObject() {
	global $lang;
	
	$cur = 14;
	$usrlevel = $_SESSION["muser_level"];
	if ($usrlevel > 0 && $usrlevel < 14) {
		if (isset ($_REQUEST["usrlevel"]))
			$cur = $_REQUEST["usrlevel"];
		else
			if ($usrlevel < 11)
				$cur = 11;
			else
				$cur = $usrlevel;
		
		$levels[$cur] = 1;
		FormListBox($lang["user_level"], "usrlevel", "1", "SearchAuthority");
		if ($_SESSION["muser_level"] < 12)
			FormListItem($lang["level11"], 11, $levels[11]);
		if ($_SESSION["muser_level"] < 13)
			FormListItem($lang["level12"], 12, $levels[12]);
		FormListItem($lang["level13"], 13, $levels[13]);
		FormListItem($lang["level14"], 14, $levels[14]);
		FormListBoxEnd();
		
		$where = " and authority = " . $cur;
		if ($_SESSION["muser_manager_id"] > 0)
			$where .= " and manager_id = " . $_SESSION["muser_manager_id"];
		if ($_SESSION["muser_region_id"] > 0)
			$where .= " and region_id = " . $_SESSION["muser_region_id"];
		if ($_SESSION["muser_district_id"] > 0)
			$where .= " and district_id = " . $_SESSION["muser_district_id"];
		if ($_SESSION["muser_affiliate_id"] > 0)
			$where .= " and affiliate_id = " . $_SESSION["muser_affiliate_id"];
		
		TBodyStart("showauthority", "1");
		FormListBoxDB($lang["user_name"], "showusers", "select system_user_id, user_name from system_user where market_skin_id=" . $_SESSION["muser_skinid"] . $where, "");
		TBodyEnd();
	}
}
function FormTimeRange($label1, $label2, $field1, $field2, $value1, $value2)
	{
	$timestamp = mktime(0,0,0, date("m"), date("d"), date("Y") );
	$newtimestamp = strtotime("-5 days",$timestamp);
	$fromdate = strftime("%d-%m-%Y",$newtimestamp);
	
	
	echo "<tr><td class='fieldlabel'>" . $label1 . "&nbsp;&nbsp;</td>";
	echo "<td><input type='text' name='" . $field1 . "' maxlength='10' size='12' value=\"$value1\" />";
	echo "<script language='JavaScript'>new tcal ({'formname': 'searchform','controlname': '" . $field1 . "'})</script>";
	echo "</td></tr>";
	
	
	}
function FormListDateHelper($label, $fromInputId)
{
	global $lang;
	echo "<tr>";
	echo "<td class=\"fieldlabel\">".$label."</td>";
	echo "<td>";
	echo "<select onchange=\"javascript:datahelp(this.value, '".$fromInputId."');\">";
	echo "<option value=\"\">".$lang["select_start_date"]."</option>";
	echo "<option value=\"ct_3600\">".$lang["1h_ago"]."</option>";
	echo "<option value=\"ct_7200\">".$lang["2h_ago"]."</option>";
	echo "<option value=\"ct_14400\">".$lang["4h_ago"]."</option>";
	echo "<option value=\"ct_28800\">".$lang["8h_ago"]."</option>";
	echo "<option value=\"ct_43200\">".$lang["12h_ago"]."</option>";
	echo "<option value=\"ct_86400\">".$lang["24h_ago"]."</option>";
	echo "<option value=\"cd_1\">".$lang["yesterday"]."</option>";
	echo "<option value=\"cd_2\">".$lang["2d_ago"]."</option>";
	echo "<option value=\"cd_4\">".$lang["4d_ago"]."</option>";
	echo "<option value=\"cw_1\">".$lang["1w_ago"]."</option>";
	echo "<option value=\"cw_2\">".$lang["2w_ago"]."</option>";
	echo "<option value=\"cm_1\">".$lang["1m_ago"]."</option>";
	echo "<option value=\"cm_2\">".$lang["2m_ago"]."</option>";
	echo "<option value=\"cm_4\">".$lang["4m_ago"]."</option>";
	echo "<option value=\"cm_6\">".$lang["6m_ago"]."</option>";
	echo "<option value=\"cm_9\">".$lang["9m_ago"]."</option>";
	echo "<option value=\"cy_1\">".$lang["1y_ago"]."</option>";
	echo "<option value=\"cy_2\">".$lang["2y_ago"]."</option>";
	echo "</select>";
	echo "</td>";
	echo "</tr>";
}
?>


