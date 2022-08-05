 import GeoTIFF, { fromUrl, fromUrls, fromArrayBuffer, fromBlob } from './geotiff.js';


 export async function readURL(input) {
	 
	 console.log(input.files[0]);
            if (input.files && input.files[0])
			{
                
				const tiff = fromBlob(input.files[0]).then(async function(resultt) 
				{
					console.log(resultt);
					console.log(resultt.getImageCount());
					const image = await resultt.getImage();
					const width = image.getWidth();
					const height = image.getHeight();
						
					const [red] = await image.readRasters();
						console.log(red);
						
						var buffer = new Uint8ClampedArray(width * height * 4);
						for(var y = 0; y < height; y++) {
							for(var x = 0; x < width; x++) {
								var pos = (y * width + x) * 4; // position in buffer based on x and y
								var posf = (y * width + x);
								var val = Math.min(255.0,red[posf]/25.0);
								if (isNaN(red[posf])) {
									val = 0;
								}else{
									val = val;
								}

								buffer[pos  ] = val;           // some R value [0, 255]
								buffer[pos+1] = val;        // some G value
								buffer[pos+2] = val;           // some B value
								buffer[pos+3] = 255;           // set alpha channel
							}
						}
						
						var canvas = document.createElement('canvas'),
						ctx = canvas.getContext('2d');
						document.body.appendChild(canvas); 
						
						canvas.width = width;
						canvas.height = height;

						// create imageData object
						var idata = ctx.createImageData(width, height);

						// set our buffer as source
						idata.data.set(buffer);

						// update canvas with new data
						ctx.putImageData(idata, 0, 0);


					/*.then(async function(image) {
						// here you can use the result of promiseB
						const width = image.getWidth();
						const height = image.getHeight();
						const tileWidth = image.getTileWidth();
						const tileHeight = image.getTileHeight();
						const samplesPerPixel = image.getSamplesPerPixel();
						document.write(Width);
						document.write(Height);

						console.log(image);
					}).catch(function(error)
					{
						console.log("error in loading tiff raster band");
					});*/
				
				})/*.catch(function(error)
				{
					console.log("error in loading tiff file");
				});*/
				
				
				/*const image = tiff.getImage(); 
				document.body.appendChild(image);
				const width = image.getWidth();
				const height = image.getHeight();
				const tileWidth = image.getTileWidth();
				const tileHeight = image.getTileHeight();
				const samplesPerPixel = image.getSamplesPerPixel();
				document.write(Width);
				document.write(Height);

				// when we are actually dealing with geo-data the following methods return
				// meaningful results:
				const origin = image.getOrigin();
				const resolution = image.getResolution();
				const bbox = image.getBoundingBox();*/
				
            }
        }