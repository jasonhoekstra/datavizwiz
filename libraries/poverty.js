var data; // loaded asynchronously

var path = d3.geo.path();

var svg = d3.select("#chart")
  .append("svg");

var states = svg.append("g")
    .attr("id", "states")
    .attr("class", "JCols");

// I'm using a modified version of this .json file
// (i.e. not the original that came in ../examples/data)
// that does not show Puerto Rico (since we don't have poverty data for PR)
d3.json("us-states.json", function(json) {
  states.selectAll("path")
      .data(json.features)
    .enter().append("path")
      .attr("class", data ? quantize : null)
      .attr("d", path);
});

d3.json("poverty.json", function(json) {
  data = json;
  states.selectAll("path")
      .attr("class", quantize)

      .on("mouseover", function(){ 
		thisstate = d3.select(this);
		states.selectAll("path").attr("class",quantize3); 
		thisstate.attr("class",quantize); 
		svg.append("text")
			.attr("transform", "translate(20,100)")
			.style("font-size","26px")
			.text( data[thisstate.property("__data__").id] + " %" );
		svg.append("text")
			.attr("transform", "translate(20,70)")
			.style("font-size","26px")
			.text( "Poverty rate:" );
	})
      .on("mouseout", function(){
		svg.selectAll("text").text("");
		states.selectAll("path").attr("class",quantize); 
	});


});


// What is this "quantize" function doing???
// 
// It seems that above in "var states ..." when we set class=Blues or Reds or whatver,
// that tells it to use colorbrewer.css to set the colors.
// The "q" here is just part of the name of the color category,
// the "-9" tells it that we want up to 9 categories,
// and the thing in between should resolve to one of the integers
// from 0 to 8 inclusive.
// So if our values range from 8.6 to 22.4,
// (X-8)/15 will force them to between 0 and 1,
// (X-8)*9/15 will force them between 0 and 9,
// and finally "~~" removes the decimal i.e. acts like a floor function
// resulting in integers from 0 to 8.
//
// And if I change it to (X-8)*8/15+1, that'll remove the lowest category
// (which is very light and thus hard to see)
// though that means now we only have 8 colors, not 9.
//
// (Note that this is a simple linear scale,
// based purely on range of data -- not on the quantiles or anything else.)
// 

function quantize(d) {
  return "q" + ~~((data[d.id] - 8) * 8 / 15 + 1) + "-9";
}


// Instead of a linear scale,
// color by whether povrate is at least 1% lower or higher than selected state
// (!!!I haven't yet adapted it to actually compare confidence intervals!!!)
function quantize3(d) {
	if(data[d.id]<data[thisstate.property("__data__").id]-1) {return "q0-3";}
	else{
	if(data[d.id]>data[thisstate.property("__data__").id]+1) {return "q2-3";}
	else{
		return "q1-3";
}
};
}
