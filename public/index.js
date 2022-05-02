//let select = "";
//let chosenFilename = "";
let currentFile = "";
let chooseFileButton = "";
let units = ["em", "ex", "px", "pt", "pc", "cm", "mm", "in", ""];
// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {

    // On page-load AJAX Example
    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/endpoint1',   //The server endpoint we are connecting to
        data: {
            data1: "Value 1",
            data2:1234.56
        },
        success: function (data) {
            // Do something with returned object
            //  Note that what we get is an object, not a string, 
            //   so we do not need to parse it on the server.
            //   JavaScript really does handle JSONs seamlessly
            
            jQuery('#blah').html("On page load, received string '"+data.somethingElse+"' from server");
            //We write the object to the console to show that the request was successful
            console.log(data); 

        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#blah').html("On page load, received error from server");
            console.log(error); 
        }
    });

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });

    const viewButtton = document.getElementById("viewButton");
    viewButton.addEventListener("click", ()=>{
          const select = document.getElementById("fileDropdownMenu");
          const chosenFilename = select.options[select.selectedIndex].text;
          selectDropdownFiles(chosenFilename);
    });

    const editSubmitButton = document.getElementById("submitEditSVG");
    editSubmitButton.addEventListener("click", ()=>{
        editSubmit();
        loadFiles();
        selectDropdownFiles(currentFile);
    });

    const editAttrSubmitButton = document.getElementById("submitEditAttribute");
    editAttrSubmitButton.addEventListener("click", ()=>{
        editAttrSubmit();
        loadFiles();
        selectDropdownFiles(currentFile);
    });

    const verifyEditButton = document.getElementById("editTitleDesc");
    verifyEditButton.addEventListener("click", ()=>{
        if (currentFile === "") {
            alert("Please select a file from the dropdown before trying to edit")
            $("#statusSection p").remove();
            $("#statusSection").append(`<p>Error: trying to use the "Edit" button without choosing a file to view first</p>`);
            console.log("Error: trying to use the 'Edit' button without choosing a file to view first");
        }
        else {
            $("#editTitleButton").modal('toggle');
        }
    });

    const verifyShowAttrButton = document.getElementById("ShowAttributeData");
    verifyShowAttrButton.addEventListener("click", ()=>{
        if (currentFile === "") {
            alert("Please select a file from the dropdown before trying to show")
            $("#statusSection p").remove();
            $("#statusSection").append(`<p>Error: trying to use the "Show" button without choosing a file to view first</p>`);
            console.log("Error: trying to use the 'Edit' button without choosing a file to view first");
        }
        else {
            $("#OtherAttributeButton").modal('toggle');
        }
    });
    
    const verifyEditAttrButton = document.getElementById("editAttributeID");
    verifyEditAttrButton.addEventListener("click", ()=>{
        if (currentFile === "") {
            alert("Please select a file from the dropdown before trying to edit")
            $("#statusSection p").remove();
            $("#statusSection").append(`<p>Error: trying to use the "Edit" button without choosing a file to view first</p>`);
            console.log("Error: trying to use the 'Edit' button without choosing a file to view first");
        }
        else {
            $("#editAttributeButton").modal('toggle');
        }
    });

    const verifyScaleButton = document.getElementById("scaleShapeBtn");
    verifyScaleButton.addEventListener("click", ()=>{
        if (currentFile === "") {
            alert("Please select a file from the dropdown before trying to scale")
            $("#statusSection p").remove();
            $("#statusSection").append(`<p>Error: trying to use the "Scale" button without choosing a file to view first</p>`);
            console.log("Error: trying to use the 'Edit' button without choosing a file to view first");
        }
        else {
            $("#scaleShapeModal").modal('toggle');
        }
    });

    const verifyAddCircleButton = document.getElementById("addCircleBtn");
    verifyAddCircleButton.addEventListener("click", ()=>{
        if (currentFile === "") {
            alert("Please select a file from the dropdown before trying to add a circle")
            $("#statusSection p").remove();
            $("#statusSection").append(`<p>Error: trying to use the "Scale" button without choosing a file to view first</p>`);
            console.log("Error: trying to use the 'Edit' button without choosing a file to view first");
        }
        else {
            $("#addCircleModal").modal('toggle');
        }
    });

    const verifyAddRectButton = document.getElementById("addRectangleBtn");
    verifyAddRectButton.addEventListener("click", ()=>{
        if (currentFile === "") {
            alert("Please select a file from the dropdown before trying to add a circle")
            $("#statusSection p").remove();
            $("#statusSection").append(`<p>Error: trying to use the "Scale" button without choosing a file to view first</p>`);
            console.log("Error: trying to use the 'Edit' button without choosing a file to view first");
        }
        else {
            $("#addRectangleModal").modal('toggle');
        }
    });

    const submitNewRectangleButton = document.getElementById("submitAddRectangle");
    submitNewRectangleButton.addEventListener("click", ()=>{
        addRectangleShape();
        loadFiles();
        selectDropdownFiles(currentFile);
    });
    const submitNewCircleButton = document.getElementById("submitAddCircle");
    submitNewCircleButton.addEventListener("click", ()=>{
        addCircleShape();
        loadFiles();
        selectDropdownFiles(currentFile);
    });
    const submitScaleShapeButton = document.getElementById("submitScaleShapes");
    submitScaleShapeButton .addEventListener("click", ()=>{
        scaleSVGShapes();
        loadFiles();
        selectDropdownFiles(currentFile);
    });
    
    // On-load files
    $.ajax ({
        type: "get",
        dataType: "json",
        url: "/SVGAttributes",
        success: function(svg) {

            $.each(svg.uploadFiles, function (i, data) {
                //console.log(`Uploaded successfully`);
                $("#fileLog").append(`<tr>
                <th scope="row"><a href="../uploads/${data.fileName}" download "><img src = "../uploads/${data.fileName}" alt = "Image of ${data.fileName}" class = "fileLogImage"</a></th>
                <th scope="row"><a href="../uploads/${data.fileName}" download ">${data.fileName}</a></th>
                <th>${data.fileSize}</th>
                <th>${data.numRect}</th>
                <th>${data.numCirc}</th>
                <th>${data.numPaths}</th>
                <th>${data.numGroups}</th>
                `);

                $("#statusSection").append(`<p>File successfully loaded: ${data.fileName}</p>`);
                console.log(`>File successfully loaded: ${data.fileName}`);
                //$("#fileDropdownMenu").append(`<option value="${data.fileName}">${data.fileName}</option>`);
            });

            if (svg.uploadFiles.length === 0) {
                $("#fileLog").append(`<tr>
                <th scope="row">No files</th>
                </tr>`);
                $("#statusSection").append(`<p>No files to load</p>`);
            }
            if (svg.failedFiles != 0) {
                console.log(
					`Files failed to upload and was removed: ${svg.failedUploads}`
				);
				$("#statusSection").append(
					`<p>Files failed to upload and was removed: ${svg.failedUploads}</p>`
				);
				alert(
					`Files failed to upload and was removed: ${svg.failedUploads}`
				);
            }
        },

        fail: function (err) {
            $("#statusSection").append(`<p> Error: Onload AJAX call for SVG file was unsuccessful</p>`);
            console.log("Error: Onload AJAX call for SVG file was unsuccessful");
        }
    });
    $.ajax ({
        type: "get",
        dataType: "json",
        url: "/SVGAttributes",
        success: function(svg) {
            console.log(svg.uploadFiles.length);
            console.log(svg);
            if (svg.uploadFiles.length !== 0) {
                $.each(svg.uploadFiles, function (i, data) {
                    if (data.fileName !== ".DS_Store") {
                        $("#fileDropdownMenu").append(`<option value="${data.fileName}">${data.fileName}</option>`);
                    }
                });
                console.log("Successfully loaded files into dropdown menu");
            }
        },
        fail: function (err) {
            console.log("Error: Failed to load files into dropdown menu");
            $("#statusSection").append(`<p>Error: Failed to load files into the dropdown menu</p>`);
        }
    })
    
    // Hide Table and header when loading up the website?

});

function loadFiles () {
    $.ajax ({
        type: "get",
        dataType: "json",
        url: "/SVGAttributes",

        success: function(svg) {
            $("#fileLog tr").remove();
            $("#fileDropdownMenu option").remove();

            $.each(svg.uploadFiles, function (i, data) {
                $("#fileLog").append(`<tr>
                <th scope="row"><a href="../uploads/${data.img}" download ">${data.img}</a></th>
                <th scope="row"><a href="../uploads/${data.fileName}" download ">${data.fileName}</a></th>
                <th>${data.fileName.size}</th>
                <th>${data.numRect}</th>
                <th>${data.numCirc}</th>
                <th>${data.numPaths}</th>
                <th>${data.numGroups}</th>`
                );
                $("#fileDropdownMenu").append(`<option value="${data.fileName}">${data.fileName}</option>`);
			});

            if (svg.uploadFiles.length === 0) {
                $("#fileLog").append(`<tr>
                <th scope="row">No files</th>
                </tr>`);
                $("#statusSection").append(`<p>No files to load</p>`);
                console.log ("No files to load");
            }

            if (svg.failedFiles != 0) {
                console.log(`Error: Files failed when uploading and they were removed: ${svg.failedFiles}`);
                $("#statusSection").append(`<p>File failed to upload and was removed: ${svg.failedFiles}`);
                alert(
					`Files failed to upload and was removed: ${svg.failedUploads}`
				);
            }
        },
        fail: function (err) {
            $("#statusSection").append(`<p>Error: AJAX call on SVG's attribute files was unsuccessful</p>`);
            console.log("Error: AJAX call on SVG's attribute files was unsuccessful");
        },
    });
}

function selectDropdownFiles(chosenFilename) {
    $("#statusSection p").remove();
    $("#statusSection").append(`<p>Opening file for view panel: ${chosenFilename}</p>`);

    currentFile = chosenFilename;
    chooseFileButton = chosenFilename;

    $.ajax ({
        type: "get",
        data: "json",
        url: "/chooseFile",

        success: function (fileInfo) {
            $("#viewPanelRows tr").remove();
            $("#viewPanelHeadRows tr").remove();
            $("#viewPanelImage tr").remove();
            $("#ContainerForComponentButtons button").remove();
            $("#ContainerForEditAttrButton button").remove();

            console.log(chosenFilename);
            
            $("#viewPanelImage").append(`<tr>
            <th scope="row"><img src = "uploads/${chosenFilename}" alt = "Image of ${chosenFilename}" class = "viewPanelImages"></th></tr>`);
            
            let uploadFile = "";
            fileInfo.attrData.forEach(file => {
                if (file.length !== 0) {
                    if (file.fileName === chosenFilename) {
                        uploadFile = file.fileName;
                        if (file.title === "" && file.description === "") {
                            $("#viewPanelHeadRows").append(`<tr>
                            <th><br></th>
                            <th><br></th>
                            </tr>`);
                        }
                        else {
                            $("#viewPanelHeadRows").append(`<tr>
                            <th>${file.title}</th>
                            <th>${file.description}</th>
                            </tr>`);
                        }
                    }
                }
            });

            $.each(fileInfo.rectData, function (i, data) {
                if (data !== "[]") {
                    $.each(data, function(i, string) {
                        if (string.fileName === chosenFilename) {
                            let summary = "";
                            // If units attribute was not empty then add it onto each string
                            if (string.units !== "") {
                                let tempX = string.x + string.units;
                                let tempY = string.y + string.units;
                                let tempW = string.w + string.units;
                                let tempH = string.h + string.units;
                                if (string)
                                summary = "x = " + tempX + ", y = " + tempY + "\nWidth = " +  tempW + ", Height = " + tempH;
                            }
                            else {
                                summary = "x = " + string.x + ", y = " + string.y + "\nWidth = " + string.w + ", Height = " + string.h;
                            }
                            $("#viewPanelRows").append(`<tr>
							<th scope="row">Rectangle ${i + 1}</th>
							<th>${summary}</th>
							<th>${string.numAttr}</th>
							</tr>`);

                            $("#ContainerForEditAttrButton").append(`<button
                            type="button"
                            id="ComponentButton"
                            class="btn btn-primary Button"
                            onclick="showAttribute(this)"
                            data-dismiss="modal"
                        >Rectangle ${i + 1}</button>`);

							// Appends the components as buttons onto the modal that shows when the "Show other data" and "Rename" button is pressed
							$("#ContainerForComponentButtons").append(`<button
								type="button"
								id="ComponentButton"
								class="btn btn-primary Button"
								onclick="showAttribute(this)"
								data-dismiss="modal"
							>Rectangle ${i + 1}</button>`);
                        }
                    });
                }
            });

            $.each(fileInfo.circData, function (i, data) {
                if (data !== "[]") {
                    $.each(data, function(i, string) {
                        if (string.fileName === chosenFilename) {
                            let summary = "";
                            // If units attribute was not empty then add it onto each string
                            if (string.units !== "") {
                                let tempX = string.cx + string.units;
                                let tempY = string.cy + string.units;
                                let tempR = string.r + string.units;
                                if (string)
                                summary = "cx = " + tempX + ", cy = " + tempY + "\nRadius = " +  tempR;
                            }
                            else {
                                summary = "cx = " + string.cx + ", cy = " + string.cy + "\nRadius = " + string.r;
                            }
                            $("#viewPanelRows").append(`<tr>
							<th scope="row">Circle ${i + 1}</th>
							<th>${summary}</th>
							<th>${string.numAttr}</th>
							</tr>`);

                            $("#ContainerForEditAttrButton").append(`<button
                            type="button"
                            id="ComponentButton"
                            class="btn btn-primary Button"
                            onclick="showAttribute(this)"
                            data-dismiss="modal"
                        >Circle ${i + 1}</button>`);

							$("#ContainerForComponentButtons").append(`<button
                            type="button"
                            id="ComponentButton"
                            class="btn btn-primary Button"
                            onclick="showAttribute(this)"
                            data-dismiss="modal"
                        >Circle ${i + 1}</button>`);
                        }
                    });
                }
            });

            $.each(fileInfo.pathData, function (i, data) {
                if (data !== "[]") {
                    $.each(data, function(i, string) {
                        if (string.fileName === chosenFilename) {
                            $("#viewPanelRows").append(`<tr>
							<th scope="row">Path ${i + 1}</th>
							<th>path data = ${string.d}</th>
							<th>${string.numAttr}</th>
							</tr>`);


                            $("#ContainerForEditAttrButton").append(`<button
                            type="button"
                            id="ComponentButton"
                            class="btn btn-primary Button"
                            onclick="showAttribute(this)"
                            data-dismiss="modal"
                        >Path ${i + 1}</button>`);

							$("#ContainerForComponentButtons").append(`<button
                            type="button"
                            id="ComponentButton"
                            class="btn btn-primary Button"
                            onclick="showAttribute(this)"
                            data-dismiss="modal"
                        >Path ${i + 1}</button>`);
                        }
                    });
                }
            });
            $.each(fileInfo.groupData, function (i, data) {
                if (data !== "[]") {
                    $.each(data, function(i, string) {
                        if (string.fileName === chosenFilename) {
                            $("#viewPanelRows").append(`<tr>
							<th scope="row">Group ${i + 1}</th>
							<th>${string.children} child elements</th>
							<th>${string.numAttr}</th>
							</tr>`);

                            $("#ContainerForEditAttrButton").append(`<button
                            type="button"
                            id="ComponentButton"
                            class="btn btn-primary Button"
                            onclick="showAttribute(this)"
                            data-dismiss="modal"
                        >Group ${i + 1}</button>`);

							$("#ContainerForComponentButtons").append(`<button
                            type="button"
                            id="ComponentButton"
                            class="btn btn-primary Button"
                            onclick="showAttribute(this)"
                            data-dismiss="modal"
                        >Group ${i + 1}</button>`);
                        }
                    });
                }
            });
			if ($("#ViewPanelRows tr").length === 0) {
				$("#ViewPanelRows").append(`<tr>
				<th scope="row">The file you have selected has no components</th>
				</tr>`);
			}
        },
        fail: function (err) {
            $("#statusSection").append(`<p>Error with get request for selecting a SVG file</p>`);
            console.log("Error with get request for selecting a SVG file");
        },
    });
}

// Function to empty forms and boxes with empty values if cancelled
function cancelButton () {
    $("#addTitleForm").val("");
    $("#addDescriptionForm").val("");
    $("#addFilenameForm").val("");
    $("#editTitleForm").val("");
    $("#editDescriptionForm").val("");
    $("#editComponentForm").val("");
    $("#editAttributeForm").val("");
    $("#editValueForm").val("");
    $("#addXForm").val("");
    $("#addYForm").val("");
    $("#addWidthForm").val("");
    $("#addHeightForm").val("");
    $("#addUnitsForm").val("");
    $("#addCXForm").val("");
    $("#addCYForm").val("");
    $("#addRadiusForm").val("");
    $("#addCircleUnitsForm").val("");
    $("#scaleRectangleForm").val("");
    $("#scaleCircleForm").val("");
}

$("#submitNewSVG").on("click", function() {
    if ($("#addTitleForm").val() === "" || $("#addDescriptionForm").val() === "" || $("#addFilenameForm").val() === "") {
        alert("Cannot have any empty fields");
    }
    else if (!$("#addFilenameForm").val().includes(".svg")) {
        alert(`Cannot create a file that does not have the ".svg" extension`)
    }
    else {
        $.ajax ({
            url: "/createSVG",
            type: "post",
            data: {
                title: $("#addTitleForm").val(),
                description: $("#addDescriptionForm").val(),
                fileName: $("#addFilenameForm").val(),
            },
            success: function(result) {
                if (result === "UNSUCCESS") {
                    alert("Failed to create the new SVG. The file was not saved.");
                    console.log("Failed to create the new SVG. The file was not saved.");
                    $("#statusSection p").remove();
                    $("#statusSection").append(`<p>Failed to create the new SVG. The file was not saved</p>`);
                    $("#addTitleForm").val("");
                    $("#addDescriptionForm").val("");
                    $("#addFilenameForm").val("");
                }
                else {
                    location.reload();
                }
            },
            fail: function (err) {
                $("#statusSection").append(`<p>An error occurred when creating the new SVG file</p>`);
                console.log("An error occurred when creating the new SVG file");
            }
        });
    }

})

function editSubmit() {
    if ($("#editTitleForm").val() === "" || $("#editDescriptionForm").val() === "") {
        alert("Cannot have any empty fields");
    }
    else {
        $.ajax({
            type: "get",
            dataType: "json",
            url: "/editSVG",
            data: {
                fileName: currentFile,
                title: $("#editTitleForm").val(),
                description: $("#editDescriptionForm").val(),
            },
    
            success: function (result) {
                if (result === "SUCCESS") {
                    $("#statusSection").append(`<p>Successfully edited title/description: ${currentFile}</p>`);
                }
                else {
                    alert("Edit was unsuccessful.");
                    $("#statusSection").append(`<p>Failed to edit title/description: ${currentFile}</p>`);
                }
            },
            fail: function (err) {
                $("#statusSection").append(`<p>Error: An error occurred while trying to edit the title/description</p>`
                );
            },
        })
    }
}

function editAttrSubmit() {
    if ($("#editComponentForm").val() === "" || $("#editAttributeForm").val() === "" || $("#editValueForm").val() === "") {
        alert("Cannot have any empty fields");
    }
    else {
        $.ajax({
            type: "get",
            dataType: "json",
            url: "/editAttributeinSVG",
            data: {
                fileName: currentFile,
                component: $("#editComponentForm").val(),
                attr: $("#editAttributeForm").val(),
                value: $("#editValueForm").val(),
            },
    
            success: function (result) {
                $("#statusSection p").remove();
                if (result === "SUCCESS") {
                    $("#statusSection").append(`<p>Successfully edited component: ${component} in ${currentFile}</p>`);
                }
                else if (result === "NEGATIVE") {
                    alert("Edit was unsuccesssful. Please enter a non-negative number for \"" + attr + "\"");
                    $("#statusSection").append(`<p>Failed to edit component: ${component} in ${currentFile}</p>`);
                }
                else {
                    alert("Edit was unsuccessful.");
                    $("#statusSection").append(`<p>Failed to edit component: ${component} in ${currentFile}</p>`);
                }
            },
            fail: function (err) {
                $("#statusSection").append(`<p>Error: An error occurred while trying to edit the title/description</p>`
                );
            },
        })
    }
}

function showAttribute(component) {
    $("#statusSection").append(`<p>Looking at component: ${component.innerHTML} from the file: ${currentFile}</p>`);

    $.ajax ({
        type: "get",
        dataType: "json",
        url: "/showOtherAttributes",
        data: {
            fileName: currentFile,
            pickedComponent: component.innerHTML,
        },

        success: function (componentAttribute) {
            console.log("Success");
            if (componentAttribute.length === 0) {
                alert(`For component: ${component.innerHTML}, no other attributes were found!`);
            }
            else {
                let attributeString = `Other attribute for component: ${component.innerHTML}\n\n`;
                let count = 0;
                $.each(componentAttribute, function (i, data) {
                    for ([key, value] of Object.entries(data)) {
                        attributeString += `${key}: ${value} `;
                        if (count % 2 !== 0) {
                            attributeString += '\n';
                        }
                        else {
                            attributeString += " | "
                        }
                        count++;
                    }
                });
                alert(attributeString);
            }
        },
        fail: function (err) {
            $("#statusSection").append(`<p>An error while retrieving the attribute of a component</p>`);
        }
    });

}

function addRectangleShape () {
    if ($("#addXForm").val() === "" || $("#addYForm").val() === "" || $("#addWidthForm").val() === "" || $("#addHeightForm").val() === "") {
        alert("Cannot have empty field(s). Please fill out all fields.");
    }
    else if (isNaN(parseFloat($("#addXForm").val())) === true) {
        alert("Cannot have a non-numerical cx");
    }
    else if (isNaN(parseFloat($("#addYForm").val())) === true) {
        alert("Cannot have a non-numerical cy");
    }
    else if (isNaN(parseFloat($("#addWidthForm").val())) === true || parseFloat($("#addWidthForm").val()) < 0) {
        alert("Invalid radius input: either non-numerical or less than 0");
    }
    else if (isNaN(parseFloat($("#addHeightForm").val())) === true || parseFloat($("#addHeightForm").val()) < 0) {
        alert("Invalid radius input: either non-numerical or less than 0");
    }
    else if (!units.includes($("#addUnitsForm").val())) {
        alert("Invalid units entered.");
    }
    else {
        $.ajax ({
            type: "get",
            dataType: "json",
            url: "/addRectangle",
            data: {
                fileName: currentFile,
                xVal: $("#addXForm").val(),
                yVal: $("#addYForm").val(),
                width: $("#addWidthForm").val(),
                height: $("#addHeightForm").val(),
                unit: $("#addUnitsForm").val(),
            },
            success: function(result) {
                $("#statusSection p").remove();
                if (result === "SUCCESS") {
                    $("#statusSection").append(`<p>Successfully added a new rectangle in file: ${currentFile}</p>`);
                }
                else if (result === "INVALID") {
                    alert("Adding new shape was unsuccesssful. Please enter valid inputs next time.");
                    $("#statusSection").append(`<p>Failed to add new rectangle in file: ${currentFile}</p>`);
                }
                else {
                    alert("Edit was unsuccessful.");
                    $("#statusSection").append(`<p>Failed to add new rectangle in file: ${currentFile}</p>`);
                }
            },
            fail: function (err) {
                $("#statusSection").append(`<p>Error: An error occurred while trying to add a new shape</p>`);
            },
        });
    }
 
}

function addCircleShape () {
    if ($("#addCXForm").val() === "" || $("#addCYForm").val() === "" || $("#addRadiusForm").val() === "") {
        alert("Cannot have empty field(s). Please fill out all fields.");
    }
    else if (isNaN(parseFloat($("#addCXForm").val())) === true) {
        alert("Cannot have a non-numerical cx");
    }
    else if (isNaN(parseFloat($("#addCYForm").val())) === true) {
        alert("Cannot have a non-numerical cy");
    }
    else if (isNaN(parseFloat($("#addRadiusForm").val())) === true || parseFloat($("#addRadiusForm").val()) < 0) {
        alert("Invalid radius input: either non-numerical or less than 0");
    }
    else if (!units.includes($("#addCircleUnitsForm").val())) {
        alert("Invalid units entered.");
    }
    else {
        $.ajax ({
            type: "get",
            dataType: "json",
            url: "/addCircle",
            data: {
                fileName: currentFile,
                cxVal: $("#addCXForm").val(),
                cyVal: $("#addCYForm").val(),
                radius: $("#addRadiusForm").val(),
                unit: $("#addCircleUnitsForm").val(),
            },
            success: function(result) {
                $("#statusSection p").remove();
                if (result === "SUCCESS") {
                    $("#statusSection").append(`<p>Successfully added a new circle in file: ${currentFile}</p>`);
                }
                else if (result === "NEGATIVE") {
                    alert("Adding new shape was unsuccesssful. Please enter a non-negative number next time.");
                    $("#statusSection").append(`<p>Failed to add new circle in file: ${currentFile}</p>`);
                }
                else {
                    alert("Edit was unsuccessful.");
                    $("#statusSection").append(`<p>Failed to add new circle in file: ${currentFile}</p>`);
                }
            },
            fail: function (err) {
                $("#statusSection").append(`<p>Error: An error occurred while trying to add a new shape</p>`);
            },
        });
    }
}

function scaleSVGShapes() {
    if ($("#scaleRectangleForm").val() === "" || $("#scaleCircleForm").val() === "") {
        alert("Cannot have empty field(s). Please fill out all fields.");
    }
    else if (isNaN(parseFloat($("#scaleRectangleForm").val())) === true) {
        alert("Cannot have a non-numerical scale for Rectangle");
    }
    else if (isNaN(parseFloat($("#scaleCircleForm").val())) === true) {
        alert("Cannot have a non-numerical scale for Circle");
    }
    else {
        $.ajax ({
            type: "get",
            dataType: "json",
            url: "/scaleShapes",
            data: {
                fileName: currentFile,
                rectScale: $("#scaleRectangleForm").val(),
                circScale: $("#scaleCircleForm").val(),
            },
            success: function(result) {
                $("#statusSection p").remove();
                if (result === "SUCCESS") {
                    $("#statusSection").append(`<p>Successfully scaled shapes in file: ${currentFile}</p>`);
                    console.log(`Successfully scaled shapes in file: ${currentFile}`);
                }
                else if (result === "NEGATIVE") {
                    alert("Scaling on shapes was unsuccesssful. Please enter a non-negative number next time.");
                    $("#statusSection").append(`<p>Failed to scale shapes in file: ${currentFile}</p>`);
                    console.log(`Failed to scale shapes in file: ${currentFile}`);
                }
                else {
                    alert("Edit was unsuccessful.");
                    $("#statusSection").append(`<p>Failed to scale shapes in file: ${currentFile}</p>`);
                    console.log(`Failed to scale shapes in file: ${currentFile}`);
                }
            },
        });
    }
}