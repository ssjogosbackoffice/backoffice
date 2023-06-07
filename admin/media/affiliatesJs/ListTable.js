function ListTable (idTable){

    this.ListTable = null;
    this.id = "#"+idTable;
    this.ListRef = $(this.id);
    this.instance = null;

    var globalid = this.id;
    this.defaultParams = {
        "iDisplayLength": 10,
        "bProcessing": true,
        "bServerSide": true,
        "bSort": false,
        "aFootersumTargets": [],
        "bFilter": false,
        "bLengthChange": true,
        "bStateSave": true,
        "sPaginationType": "full_numbers",
        "iCookieDuration": 0,
        fnRowCallback: function( nRow, aData, iDisplayIndex, iDisplayIndexFull ) {
            // Row click
            $(nRow).css("cursor", "pointer");
            $('td:lt(6)', nRow).on('click', function() {
                getAffiliateTree(aData);
            });
            $('#aff_'+aData[0], nRow).on('click', function() {

                $.ajax({
                    url:$url,
                    type:"POST",
                    data:"action=activate&affiliate="+aData[1],
                    success:function(data, textStatus, jqXHR){
//    / data = $.parseJSON(data);
                        if($.trim(data) == "0"){
                            alert('Successfully activated affiliate');
                        }else{
                            alert('Affiliate not activated');
                        }
                        ListTable.getInstance().update();
                    },
                    error:function(jqXHR, textStatus, errorThrown){
                        alert('Affiliate not activated');
                    }
                });
            });
            $('#aff_'+aData[0]+'_deact', nRow).on('click', function() {
                $.ajax({
                    url:$url,
                    type:"POST",
                    data:"action=deactivate&affiliate="+aData[1],
                    success:function(data, textStatus, jqXHR){
//    / data = $.parseJSON(data);

                        if(data.status == 1){
                            alert('Successfully activated affiliate');
                        }else{
                            alert('Affiliate not activated');
                        }
                        ListTable.getInstance().update();
                    },
                    error:function(jqXHR, textStatus, errorThrown){
                        alert('Affiliate not activated');
                    }
                });
            });

            return nRow;
        },
        "fnStateSave": function ( oSettings, sValue ) {

            var idFilter = globalid+"_formFilter";
            $(idFilter).find('input,select').each(function(){
                if($(this).attr('save') != 'false'){
                    sValue += ',"'+$(this).attr('name')+'": "'+$(this).attr('value')+'"';
                }
            });
            return sValue;
        },
        "fnStateLoad": function ( oSettings, oData ) {
            var idFilter = globalid+"_formFilter";
            $(idFilter).find('input,select').each(function(){
                if($(this).attr('save') != 'false'){

                    var index = $(this).attr("name");

                    $(this).val(oData[index]);
                }
            });
            return true;
        },
        "fnServerData": function ( sSource, aoData, fnCallback ) {

            var idFilter = globalid+"_formFilter";
            $(idFilter).find('input,select').each(function(){
                if($(this).attr('type') == "radio"){
                    if($(this).is(":checked")){
                        aoData.push({
                            "name":$(this).attr('name'),
                            "value":$(this).attr('value')
                        });
                    }
                }
                else{
                    if($(this).attr('send') != 'false'){
                        aoData.push({
                            "name":$(this).attr('name'),
                            "value":$(this).attr('value')
                        });
                    }
                }

            });
            $.ajax( {
                "async":false,
                "type": "POST",
                "url": sSource,
                "data": aoData,
                "success": function(json){
                    var array = json.split("^^");
                    if(array.length>1){
                        fillElement(array[1]);
                    }
                    if(json.error != null){
                        alert("Attenzione: "+ json.error);
                    }
                    fnCallback($.parseJSON(array[0]));
                }
            } );

        },
        "fnFooterCallback": function ( nRow, aaData, iStart, iEnd, aiDisplay ) {
            var settings = this.fnSettings();
            if ( settings.nTFoot !== null && settings.oInit.aFootersumTargets.length > 0){
                $.each(settings.oInit.aFootersumTargets, function(ind,e){
                    var iTotal = 0;
                    for ( var i=iStart ; i<iEnd ; i++ ){
                        if(aaData[i])
                            iTotal += aaData[i][e-1]*1;
                    }

                    var nCells = $(settings.nTFoot).find('th');
                    $(nCells[(e-1)]).text(iTotal.toFixed(2));
                });


            }
        }
    };
};

ListTable.getInstance = function (idTable){
    if(this.instance == null)
        this.instance = new ListTable(idTable);
    return this.instance;
};

ListTable.prototype.editColumns = function(obj, columns)
{
    var table = this.ListTable;
    for(var a = 0; a < columns.length; a++)
    {
        var column = columns[a];
        $('td:eq('+column.id+')', this.ListTable.fnGetNodes()).editable( this.defaultParams.sAjaxSource, {
            indicator : 'Saving...',
            tooltip   : 'Click to edit...',
            type : column.type,
            data : column.data,
            submit: column.submit,
            loadurl : column.url,
            loadtype: "POST",
            loaddata: function(value, settings) {
                return {
                    "row_id": table.fnGetData(table.fnGetPosition( this )[0])[0],
                    "type": column.loadData,
                    "action": "getSelectData"
                };
            },
            callback: function( sValue, y ) {
                var aPos = table.fnGetPosition( this );
                table.fnUpdate( sValue, aPos[0], aPos[1] );
                obj.editColumns(obj,columns);
            },
            submitdata: function ( value, settings ) {
                return {
                    "row_id": table.fnGetData(table.fnGetPosition( this )[0])[0],
                    "column": table.fnGetPosition( this )[2],
                    "action": column.func
                };
            },
            "height": "25px"
        } );
    }
};

ListTable.prototype.setTarget = function(target){
    this.defaultParams.sAjaxSource = target;
};

ListTable.prototype.setParam = function(param,value){
    this.defaultParams[param] = value;
};

ListTable.prototype.getCountRow = function(){
    return this.ListTable.fnGetData().length;
}

ListTable.prototype.update = function(){
    var setting = this.ListTable.fnSettings();
    //setting._iDisplayStart=0;
    this.ListTable.fnDraw();
};

ListTable.prototype.isCreate = function()
{
    return (this.ListTable != null);
};

ListTable.prototype.create = function(){
    this.ListTable = this.ListRef.dataTable(this.defaultParams);

    $(".date").datepicker({
        buttonText:"Data from",
        dateFormat: 'dd/mm/yy',
        constrainInput: true
    });
};

ListTable.prototype.regenerate = function(clear){

    this.ListTable.fnDestroy(true);
    var ind = this.id+"_formFilter";
    if(clear)
        this.ListRef.html('');
    $(ind).after(this.ListRef);
    this.create();
    this.ListRef.css('width', '');

};
