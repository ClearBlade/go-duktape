var myThing = { name: "yo wut up" };
console.log("hi");
console.log("myThing", JSON.stringify(myThing));
console.log("before timeout");
setTimeout(function() {
  console.log("inside timeout");
  myThing.name = "yo!!!";
  var id = setTimeout(function() {
    console.log("inside second timeout");
  }, 2000);
  console.log("after first", id);
  setTimeout(function() {
    console.log("inside faster timeout");
    console.log("myThing", JSON.stringify(myThing));
    clearTimeout(id);
  }, 1000);
}, 1000);
console.log("after timeout");
