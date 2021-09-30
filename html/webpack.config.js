const path = require('path');
const webpack = require('webpack'); //to access built-in plugins
const CopyPlugin = require("copy-webpack-plugin");

module.exports = {
  entry: './src/example.js',
  output: {
    filename: 'main.js',
    path: path.resolve(__dirname, 'dist'),
  },
  optimization: {
        minimize: false
  },
  plugins: [
	new webpack.ProvidePlugin({
		$: 'jquery',
		jQuery: 'jquery',
	}),
	new CopyPlugin({
      patterns: [
        { from: "src/command.proto", to: "/opt/repos/fm_smoov/html/dist/command.proto" },
      ],
    }),
  ],
  //devtool: "inline-source-map",
	  
};
