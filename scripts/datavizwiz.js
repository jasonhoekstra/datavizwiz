/* Tab Functions */

$(document).ready(function(){
  initTabs();
});
 
function initTabs() {
  $('#dvwTabs a').bind('click',function(e) {
    event.preventDefault ? event.preventDefault() : event.returnValue = false;
    var thref = $(this).attr("id").replace(/#/, '');
    thref = 'info' + thref;
    $('#dvwTabs a').removeClass('dvwActive');
    $(this).addClass('dvwActive');
    //$('#dvwTabContent div.dvwContent').removeClass('dvwActive');
    $('#dvwTabContent div.dvwContent').css('left', '-20000px');
    $('#'+thref).css('left', '0px');
  });
}

/* Flot Pie and Bar Functions */

function flotPie(paneID, dataset) {
  $.plot($('#' + paneID), dataset
    , {
      series: {
        pie: {
          show: true, 
          radius: 0.95,  
          label: {
            show: true,
            radius: 2/3,
            formatter: function(label, series){
              return '<div class="dvwPieLabel">'
              +label+
              '<br/><br/>'
              +Math.round(series.percent)+
              '%</div>';
            },
            threshold: 0.1
          }
        }
      },
      legend: {
        show: true, 
        formatter: function(x,y) { alert('test'); },
        labelFormatter: function(label, series) {
          var percent= Math.round(series.percent);
          var number= series.data[0][1];
          return(''+label+'</td><td>'+number+'</td><td>'+ percent + '%');
        }
      },
      grid: {
        clickable: true
      }
    }
    );
}

function flotBar(paneID, variable, dataset, tickset) {
  $.plot(
    $('#' + paneID),
    [ {
      label: variable, 
      data: dataset, 
      color: "#08519C",
      bars: { show: true, barWidth: 0.3, align: "center" },
      grid: { hoverable: true }
    } ],
    { xaxis: { ticks: tickset } }
  );
  
  var previousPoint = null;
  $("#" + paneID).bind("plothover", function (event, pos, item) {
    console.log(event);
    console.log(pos);
    console.log(item);
    alert('test');
    
    if (item) {
        if (previousPoint != item.dataIndex) {
            previousPoint = item.dataIndex;

            $("#tooltip").remove();
            var x = item.datapoint[0].toFixed(2),
                y = item.datapoint[1].toFixed(2);

            showTooltip(item.pageX, item.pageY,
                        item.series.label + " of " + x + " = " + y);
        }
    }
    else {
        $("#tooltip").remove();
        previousPoint = null;            
    }
  });
}

function showTooltip(x, y, contents) {
  $('<div id="tooltip">' + contents + '</div>').css( {
      position: 'absolute',
      display: 'none',
      top: y + 5,
      left: x + 5,
      border: '1px solid #fdd',
      padding: '2px',
      'background-color': '#fee',
      opacity: 0.80
  }).appendTo("body").fadeIn(200);
}

/* OpenLayers Functions */

var selectControl;
var selectedFeature;
var map;

function initOpenLayersMap(pane_id, geoDataSource) {
  var streets = new OpenLayers.Layer.XYZ(
    "MapBox Streets",
    [
    "http://a.tiles.mapbox.com/v3/mapbox.mapbox-streets/${z}/${x}/${y}.png",
    "http://b.tiles.mapbox.com/v3/mapbox.mapbox-streets/${z}/${x}/${y}.png",
    "http://c.tiles.mapbox.com/v3/mapbox.mapbox-streets/${z}/${x}/${y}.png",
    "http://d.tiles.mapbox.com/v3/mapbox.mapbox-streets/${z}/${x}/${y}.png"
    ], {
      sphericalMercator: true,
      wrapDateLine: true,
      transitionEffect: "resize",
      buffer: 1,
      numZoomLevels: 17
    }
    );

    map = new OpenLayers.Map({
    div: pane_id,
    layers: [streets],
    projection: "EPSG:4326",
    zoom: 4,
    controls: [
    new OpenLayers.Control.Navigation({
      dragPanOptions: {
        enableKinetic: true
      }
    }),
    new OpenLayers.Control.Attribution(),
    new OpenLayers.Control.Zoom()
    ]
  });

  map.setCenter(
    new OpenLayers.LonLat(-98.35, 39.50).transform(
      new OpenLayers.Projection("EPSG:4326"),
      map.getProjectionObject()
      ), 4
    );

  $.ajax({
    url: geoDataSource, 
    success: handleGeoJSON, 
    dataType: "json"
  });
}

//  *** this fires when you move over a feature ***
function onFeatureSelect(feature) {       
  var htmlString = "";
  for (i=0; i<feature.cluster.length; i++) {
    htmlString = htmlString + feature.cluster[i].attributes.popupContent + "<br/>";
  }
  
  if (htmlString != "") {
    $("#dvw-infobox").show();
    $("#dvw-infobox").html(htmlString); 
  }
}

//  *** this fires when you move off a feature ***
function onFeatureUnselect(feature) {
  $("#dvw-infobox").html("");
  $("#dvw-infobox").hide();
}

function handleGeoJSON(geodata) {
  console.log(geodata);
  baseProjection = map.projection; 
  wgs84 = new OpenLayers.Projection("EPSG:4326");
  diff = new OpenLayers.Projection("EPSG:900913");
  var geojson_format = new OpenLayers.Format.GeoJSON({
    "externalProjection": wgs84,
    "internalProjection": diff
  });
  
  var style = new OpenLayers.Style({
    pointRadius: "${radius}",
    fillColor: "#66ccff",
    fillOpacity: 0.8,
    strokeColor: "#3366cc",
    strokeWidth: 2,
    strokeOpacity: 0.8,
    property: "class"
  }, {
    context: {
      radius: function(feature) {
        return Math.min(feature.attributes.count, 20) + 3;
      }
    }
  });

  var selectedStyle = new OpenLayers.Style({
    fillColor: "#cccc33",
    strokeColor: "#cc66cc"
  });

  var vector_layer = new OpenLayers.Layer.Vector("Locations", {
    projection: "EPSG:4326",
    strategies: [
    new OpenLayers.Strategy.Cluster() //{distance: 15, threshold: 3}
    ],
    styleMap: new OpenLayers.StyleMap({
      "default": style, 
      "select": selectedStyle
    })
  }
  ); 

  map.addLayer(vector_layer);
  var vector_data = geojson_format.read(geodata);
  console.log(vector_data);
  vector_layer.addFeatures(vector_data);
        
  // Used to display the dialog popup
  selectControl = new OpenLayers.Control.SelectFeature(vector_layer,
  {
    hover: true,
    onSelect: onFeatureSelect,
    onUnselect: onFeatureUnselect 
  });
  map.addControl(selectControl);
  selectControl.activate();
}
