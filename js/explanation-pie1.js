(function (){
    var width = 640,
    height = 400,
    radius = Math.min(width, height) / 2;

    var color = d3.scale.category10();

    var dataset = [{name: "a", before: 4, after: 0},
                   {name: "b", before: 2, after: 2},
                   {name: "c", before: 3, after: 3},
                   {name: "d", before: 6, after: 6},
                   {name: "e", before: 2, after: 2},
                   {name: "f", before: 3, after: 3}];

    var arc = d3.svg.arc()
        .outerRadius(radius - 10)
        .innerRadius(radius - 70);

    var pie = d3.layout.pie()
        .value(function(d) { return d.before; })
        .sort(null);

    var svg = d3.select("#pie1").append("svg")
        .attr("width", width)
        .attr("height", height)
        .append("g")
        .attr("transform", "translate(" + width / 2 + "," + height / 2 + ")");

    var g = svg.datum(dataset).selectAll(".arc")
        .data(pie)
        .enter().append("g")
        .attr("class", "arc");

    g.append("path")
        .attr("d", arc)
        .style("fill", function(d) { return color(d.data.name); })
        .each(function(d) { this._current = d; });

    g.append("text")
        .attr("transform", function(d) { return "translate(" + arc.centroid(d) + ")"; })
        .attr("dy", ".35em")
        .style("text-anchor", "middle")
        .text(function(d) { return d.data.name; })
        .each(function(d) { this._current = d; });

    var path = svg.selectAll("path");
    var text = svg.selectAll("text");

    d3.selectAll(".pie1form").on("change", change);

    function change() {
        var value = this.value;
        pie.value(function(d) { return d[value]; });
        path = path.data(pie);
        path.transition().duration(750).attrTween("d", arcTween);
        text = text.data(pie);
        text.transition().duration(750).attrTween("transform", textTween);
    }

    function genericTween(f) {
        return function(a) {
            var i = d3.interpolate(this._current, a);
            this._current = i(0);
            return function(t) {
                return f(i(t));
            };
        };
    }

    var textTween = genericTween(
        function(v) {
            return "translate(" + arc.centroid(v) + ")";
        });

    var arcTween = genericTween(arc);
})();
