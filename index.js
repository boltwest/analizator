const fs = require("fs")
const SerialPort = require('serialport')
const Readline = require('@serialport/parser-readline')
const port = new SerialPort('COM3', {
  baudRate: 9600
}, function(err) {
    if (err) {
        return console.log('Error opening port: ', err.message)
    }
    console.log('Opening port');
})

const parser = port.pipe(new Readline({ delimiter: '\r\n' }));

const month = process.argv[2] || 0;
const day = process.argv[3] || 0;
const hours = process.argv[4] || 0;
const minutes = process.argv[5] || 0;
console.log(`month: ${month}\tday: ${day}\thours: ${hours}\tminutes: ${minutes}`);
const date = new Date(2019, month, day, hours, minutes);
const array = [];

parser.on('data', function (data) {
    if (data === 'end') {
        // console.log('end');
        port.close();
        writeFile(array);
        return;
    }
    const d = data.split(':')
    const time = Number(d[0]);
    const temperature = Number(d[1]);
    const currentDate = new Date(date.getTime() + time)
    let h = currentDate.getHours();
    h = h < 10 ? `0${h}` : h;
    let m = currentDate.getMinutes();
    m = m < 10 ? `0${m}` : m;
    // console.log(date);
    // console.log(date.toJSON());
    const shortTime = `${h}:${m}`
    console.log(shortTime, '\t', temperature);
    array.push({time: shortTime, t: temperature});

    // console.log(data);
})
function writeFile(arr) {
    fs.writeFileSync(`./data/2019_${month}_${day}.json`, JSON.stringify(arr))
}
