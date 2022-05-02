'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

//Sample endpoint
app.get('/endpoint1', function(req , res){
  let retStr = req.query.data1 + " " + req.query.data2;
 
  res.send(
    {
      somethingElse: retStr
    }
  );
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);


let sharedLib = ffi.Library("./libsvgparser.so", {
  'SVGFiletoJSON': ['string', ['string', 'string']],
  'convertAttributeToJSON': ['string', ['string', 'string']],
  'convertTitleToJSON': ['string', ['string', 'string']],
  'convertDescToJSON': ['string', ['string', 'string']],
  'SVGDataToRectList': ['string', ['string', 'string']],
  'SVGDataToCircleList': ['string', ['string', 'string']],
  'SVGDataToPathList': ['string', ['string', 'string']],
  'SVGDataToGroupList': ['string', ['string', 'string']],
  'JSONcreateSVG': ['bool', ['string', 'string']],
  'JSONeditSVG': ['bool', ['string', 'string', 'string']],
  'SVGRectsAttrListToJSON': ['string', ['string']],
  'SVGCircleAttrListToJSON': ['string', ['string']],
  'SVGPathAttrListToJSON': ['string', ['string']],
  'SVGGroupAttrListToJSON': ['string', ['string']],
  'JSONeditAttrSVG': ['bool', ['string', 'string', 'int', 'string', 'string']],
  'AddRectangleToJSON': ['bool', ['string', 'string', 'string', 'string', 'string', 'string']],
  'AddCircleToJSON': ['bool', ['string', 'string', 'string', 'string', 'string']],
  'validUnits': ['bool', ['string']],
  'scaleShapes': ['bool', ['string', 'string', 'string']],
});

// Get request used to find the attributes required for the File Log panel and for each file given
app.get("/SVGAttributes", function (req, res) {
    let files = fs.readdirSync("uploads");
    let failedFiles = [];
    let uploadFiles = [];

    files.forEach((file) => {
      let path = "uploads/" + file;
      let parsedStr = "";
      let JSONstr = sharedLib.SVGFiletoJSON(path, "parser/src/svg.xsd");

      // If the SVG sent was empty then remove it from the uploads directory and push it into failedFiles
      if (JSONstr === '{}') {
        fs.unlinkSync(path);
        failedFiles.push(file);
      }
      else {
        parsedStr = JSON.parse(JSONstr.replaceAll("\n", "\\n"));
        parsedStr.fileName = file;
        uploadFiles.push(parsedStr);
      }
    });
    res.send ({
      uploadFiles: uploadFiles,
      failedFiles: failedFiles,
    });
});

app.get("/chooseFile", function(req, res) {
  let files = fs.readdirSync("uploads/");
  let attrFileArray = [];
  let rectFileArray = [];
  let circFileArray = [];
  let pathFileArray = [];
  let groupFileArray = [];

  files.forEach((file) => {
    let path = "uploads/" + file;
    let title = "";
    let descr = "";

    title = sharedLib.convertTitleToJSON(path, 'svg.xsd');
    descr = sharedLib.convertDescToJSON(path, 'svg.xsd');

    let array = {};
    array.title = title;
    array.description = descr;
    array.fileName = file;

    attrFileArray.push(array);
  });
  // Traverse through the files and find the rectangle elements
  files.forEach((file) => {
    let path = "uploads/" + file;

    let array = sharedLib.SVGDataToRectList(path, 'svg.xsd');
    let parsedArray = JSON.parse(array.replaceAll("\n", "\\n"));

    parsedArray.forEach((string) => {
      string.fileName = file;
    });
    rectFileArray.push(parsedArray);
  });
  // Traverse throguh files and find circle elements
  files.forEach((file) => {
    let path = "uploads/" + file;

    let array = sharedLib.SVGDataToCircleList(path, 'svg.xsd');
    let parsedArray = JSON.parse(array.replaceAll("\n", "\\n"));

    parsedArray.forEach((string) => {
      string.fileName = file;
    });
    circFileArray.push(parsedArray);
  });
  // Traverse through files and find path elements
  files.forEach((file) => {
    let path = "uploads/" + file;

    let array = sharedLib.SVGDataToPathList(path, 'svg.xsd');
    let parsedArray = JSON.parse(array.replaceAll("\n", "\\n"));

    parsedArray.forEach((string) => {
      string.fileName = file;
    });
    pathFileArray.push(parsedArray);
  });
  //Traverse through files and find group elements
  files.forEach((file) => {
    let path = "uploads/" + file;

    let array = sharedLib.SVGDataToGroupList(path, 'svg.xsd');
    let parsedArray = JSON.parse(array.replaceAll("\n", "\\n"));

    parsedArray.forEach((string) => {
      string.fileName = file;
    });
    groupFileArray.push(parsedArray);
  });

  console.log("Successfully sent data to /chooseFile")

  res.send ({
    attrData: attrFileArray,
    rectData: rectFileArray,
    circData: circFileArray,
    pathData: pathFileArray,
    groupData: groupFileArray,
  });
});

// Call to send the form data into the body of the post request
app.use(express.urlencoded({extended: true}));

app.post("/createSVG", function(req, res) {
  let svgObject = {title: req.body.title, descr: req.body.description};
  let file = "uploads/" + req.body.fileName;

  let valid = sharedLib.JSONcreateSVG(JSON.stringify(svgObject), file);

  if (valid) {
    console.log("Successful post request was made to create a new SVG file.");
    res.send("SUCCESS");
  }
  else {
    console.log("Unsuccessful post request was made to create SVG file.");
    res.send("UNSUCCESS");
  }
});

app.get("/editSVG", function(req, res) {
  let valid = sharedLib.JSONeditSVG(`uploads/${req.query.fileName}`, req.query.title, req.query.description);

  if (valid) {
    res.send("SUCCESS");
  }
  else {
    res.send("UNSUCCESS");
  }
});


app.get("/editAttributeinSVG", function(req, res) {
  let type = "";
  let index = "";
  let attribute = "";
  attribute = req.query.attr;

  if (req.query.component.toLowerCase().includes("rectangle")) {
    index = req.query.component[10];
    type = "rectangle";
  }
  else if (req.query.component.toLowerCase().includes("circle")) {
    index = req.query.component[7];
    type = "circle";
  }
  else if (req.query.component.toLowerCase().includes("path")) {
    index = req.query.component[5];
    type = "path";
  }
  else if (req.query.component.toLowerCase().includes("group")) {
    index = req.query.component[6];
    type = "group";
  }
  else if (req.query.component.toLowerCase().includes("svg")) {
    index = 0;
    type = "svg";
  }
  else {
    res.send("UNSUCCESS");
  }

  if (typeof type == 'undefined' || typeof attribute == 'undefined') {
    res.send("UNSUCCESS");
  }
  if (type === "rectangle") {
    if (attribute.toLowerCase() === "w" || attribute.toLowerCase() === "h") {
      if (parseInt(req.query.value) < 0) {
        res.send("NEGATIVE");
      }
    }
  }
  if (type === "circle") {
    if (attribute.toLowerCase() === "r") {
      if (parseInt(req.query.value) < 0) {
        res.send("NEGATIVE");
      }
    }
  }
  
  index = index - 1;
  let valid = sharedLib.JSONeditAttrSVG(`uploads/${req.query.fileName}`, type, index, req.query.attr, req.query.value);
  if (valid) {
    res.send("SUCCESS");
  }
  else {
    res.send("UNSUCCESS");
  }
});


app.get("/showOtherAttributes", function(req, res) {
  let attrArray = [];

  if (req.query.pickedComponent.includes("Rectangle")) {
    let rectAttrArray = sharedLib.SVGRectsAttrListToJSON(`uploads/${req.query.fileName}`);
    let rectNumber = req.query.pickedComponent[10];

    rectAttrArray = rectAttrArray.replaceAll("\n", "\\n");
    let rectAttributes = JSON.parse(rectAttrArray);

    attrArray = rectAttributes[rectNumber - 1];
  }
  else if (req.query.pickedComponent.includes("Circle")) {
    let circAttrArray = sharedLib.SVGCircleAttrListToJSON(`uploads/${req.query.fileName}`);
    let circNumber = req.query.pickedComponent[7];

    circAttrArray = circAttrArray.replaceAll("\n", "\\n");
    let circAttributes = JSON.parse(circAttrArray);

    attrArray = circAttributes[circNumber - 1];
  }
  else if (req.query.pickedComponent.includes("Path")) {
    let pathAttrArray = sharedLib.SVGPathAttrListToJSON(`uploads/${req.query.fileName}`);
    let pathNumber = req.query.pickedComponent[5];

    pathAttrArray = pathAttrArray.replaceAll("\n", "\\n");
    let pathAttributes = JSON.parse(pathAttrArray);

    attrArray = pathAttributes[pathNumber - 1];
  }
  else if (req.query.pickedComponent.includes("Group")) {
    let groupAttrArray = sharedLib.SVGPathAttrListToJSON(`uploads/${req.query.fileName}`);
    let groupNumber = req.query.pickedComponent[6];

    groupAttrArray = groupAttrArray.replaceAll("\n", "\\n");
    let groupAttributes = JSON.parse(groupAttrArray);

    attrArray = groupAttributes[groupNumber - 1];
  }
  res.send(attrArray);
});

app.get("/addRectangle", function(req, res) {
  let path = `uploads/${req.query.fileName}`;

  if (req.query.unit !== "") {
    let validUnits = sharedLib.validUnits(req.query.unit);

    if (!validUnits) {
      res.send("INVALID");
    }
  }
  if (parseInt(req.query.width) < 0 || parseInt(req.query.height) < 0) {
    res.send("INVALID");
  }

  let valid = sharedLib.AddRectangleToJSON(path, req.query.xVal, req.query.yVal, req.query.width, req.query.height, req.query.unit);

  if (valid) {
    res.send("SUCCESS");
  }
  else {
    res.send("UNSUCCESS");
  }
});

app.get("/addCircle", function(req, res) {
  let path = `uploads/${req.query.fileName}`;

  if (req.query.unit !== "") {
    let validUnits = sharedLib.validUnits(req.query.unit);

    if (!validUnits) {
      res.send("INVALID");
    }
  }
  if (parseInt(req.query.radius) < 0) {
    res.send("INVALID");
  }

  let valid = sharedLib.AddCircleToJSON(path, req.query.cxVal, req.query.cyVal, req.query.radius, req.query.unit);

  if (valid) {
    res.send("SUCCESS");
  }
  else {
    res.send("UNSUCCESS");
  }
});


app.get("/scaleShapes", function(req, res) {
  let path = `uploads/${req.query.fileName}`;
  let rScale = req.query.rectScale;
  let cScale = req.query.circScale;

  if (rScale === "" || cScale === "") {
    res.send("UNSUCCESS");
  }
  if (parseInt(rScale) < 0 || parseInt(cScale) < 0) {
    res.send("INVALIID");
  }
  else {
    let valid = sharedLib.scaleShapes(path, rScale, cScale);

    if (valid) {
      res.send("SUCCESS");
    }
    else {
      res.send("UNSUCCESS");
    }
  }
  
});