
print "Start";

numbersAreBooleansHere = 1;

// if-statements are just opeators.
//The last exprssion is concidered the value.
x = if numbersAreBooleansHere { 0; } else { 1 ;};

someTable = { x = x; y = 20; };
someArray = array {someTable, 43};

// Tables and arrays (like in JS) are passed by "address".
someFunction = fn(tbl) { tbl.x = 1 + tbl.x * 2; };

someFunction(someTable);
print someArray;

print "End";