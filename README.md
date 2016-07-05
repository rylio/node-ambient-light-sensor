# Ambient-Light-Sensor

Access the ambient light sensor on macOS in node.js.

## Install

      npm install --save ambient-light-sensor

## Example

```javascript
const als = require('ambient-light-sensor');
als(function(err, data) {
   if(!err) {
      console.log(data.computedValue, data.rawValue);
   }
});
```

## License

ambient-light-sensor is released under the MIT license. See [LICENSE](https://github.com/otium/node-ambient-light-sensor/blob/master/LICENSE) for details.
