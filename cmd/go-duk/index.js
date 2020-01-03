var myThing = { name: "yo wut up" };
console.log(typeof my_timers);
console.log(typeof timer_id);
console.log("hi");
console.log("myThing", JSON.stringify(myThing));
console.log("before timeout");
// throw new Error("ow!");
setTimeout(function() {
  console.log("inside timeout");
  // throw new Error("ow");
  myThing.name = "yo!!!";
  var id = setTimeout(function() {
    console.log("inside second timeout");
  }, 2000);
  console.log("after first", id);
  setTimeout(function() {
    console.log("inside faster timeout");
    console.log("myThing", JSON.stringify(myThing));
    clearTimeout(id);
  }, 2000);
}, 1000);
console.log("after timeout");
