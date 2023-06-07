<?php
if(isset($_SESSION['admin_id'])){
$html= preg_replace('/<a href=\"(.*?)\">(.*?)<\/a>/', "\\2", $_POST['pdfData']);
include  "FPDF/mpdf.php";
$pdf = new mPdf($mode='',$format='A4-L');
$stylesheet = file_get_contents(secure_host ."/media/style.css"); // external css
$stylesheet.= file_get_contents(secure_host ."/media/table.css"); // external css
$stylesheet.= file_get_contents(secure_host ."/media/pdfStyle.css");
$pdf->WriteHTML($stylesheet,1);
$pdf->WriteHTML($html,2);
$pdf->Output();
    }
elsE{
    die('Please login');
}
?>