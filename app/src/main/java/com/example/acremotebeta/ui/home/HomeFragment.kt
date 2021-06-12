package com.example.acremotebeta.ui.home

import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.Switch
import android.widget.TextView
import android.widget.Toast
import androidx.fragment.app.Fragment
import androidx.lifecycle.ViewModelProviders
import com.example.acremotebeta.R
import com.google.android.material.slider.Slider
import org.json.JSONObject
import org.json.JSONTokener

import java.io.BufferedReader
import java.io.IOException
import java.io.InputStreamReader
import java.net.*
import java.util.*
import java.util.concurrent.Executors

const val STATUS_HANDLER=1
const val SYNC_HANDLER=0

var ipAddRemote:String=""
var handler = object : Handler(Looper.getMainLooper()) {


}
class HomeFragment (): Fragment() {

    val executor = Executors.newFixedThreadPool(3)
    private lateinit var homeViewModel: HomeViewModel
    lateinit var mainHandler: Handler
    lateinit var updateTextTask:Runnable

    override fun onCreateView(
            inflater: LayoutInflater,
            container: ViewGroup?,
            savedInstanceState: Bundle?
    ): View? {
        homeViewModel = ViewModelProviders.of(this).get(HomeViewModel::class.java)
        val root = inflater.inflate(R.layout.fragment_home, container, false)
        val textViewTempEnv: TextView = root.findViewById(R.id.text_temp_env)
        val textViewHumEnv: TextView = root.findViewById(R.id.text_hum_env)
        val textViewSetTemp: TextView = root.findViewById(R.id.text_set_temp)
        val sliderSetTemp: Slider = root.findViewById(R.id.tempSlider)
        val syncButton: Button = root.findViewById(R.id.syncButton)
        val switchOnOff:Switch=root.findViewById(R.id.switchOnOff)




        /*homeViewModel.text.observe(viewLifecycleOwner, Observer {
            textView.text = it
        })*/

        Toast.makeText(activity,"this is toast message", Toast.LENGTH_SHORT).show()
         handler = object : Handler(Looper.getMainLooper()) {
            /*
             * handleMessage() defines the operations to perform when
             * the Handler receives a new Message to process.
             */
            override fun handleMessage(inputMessage: Message) {
                // Gets the image task from the incoming Message object.

                //Toast.makeText(activity,"Set Temp is "+sliderSetTemp.value, Toast.LENGTH_SHORT).show()
                if(inputMessage.what== SYNC_HANDLER){

                    var jsonObj=JSONObject(JSONTokener(inputMessage.obj as String))
                    Toast.makeText(activity,"Input msg: "+jsonObj.get("ip"), Toast.LENGTH_SHORT).show()
                    ipAddRemote=jsonObj.get("ip") as String
                    mainHandler.post(updateTextTask)



                }else if(inputMessage.what== STATUS_HANDLER)
                {
                    var jsonObj=JSONObject(JSONTokener(inputMessage.obj as String))
                    //Toast.makeText(activity,"Input msg: "+jsonObj.get("setTempAc"), Toast.LENGTH_SHORT).show()
                    var powerStatus:Boolean=jsonObj.get("powerStatus") as Boolean
                    var setTempAc:Double=jsonObj.get("setTempAc") as Double
                    var tempEnv:Double=jsonObj.get("tempEnv") as Double
                    var humEnv:Double=jsonObj.get("humEnv") as Double
                        //setTempAc=jsonObj.get("setTempAc") as Float

                    Log.i("Json Obj",jsonObj.toString())

                    //val tempSets:Int=jsonObj.get("setTempAc")as Float
                    switchOnOff.isChecked=powerStatus
                    sliderSetTemp.value= setTempAc.toFloat()
                    textViewSetTemp.setText(setTempAc.toString()+"\u00B0C")
                    //                    //sliderSetTemp.value= 30.0f
                    textViewHumEnv.setText("%.2f".format(humEnv)+"%")
                    textViewTempEnv.setText("%.2f".format(tempEnv)+"\u00B0C")

                }

            }
        }
        mainHandler = Handler(Looper.getMainLooper())
        val tcpServerRunnable=TcpServerRunnable(handler)
        val tcpClientRunnable=TcpClientRunnable(handler, ipAddRemote,8888)
        updateTextTask = object : Runnable {
            override fun run() {
                //Toast.makeText(activity,"Testing repeat", Toast.LENGTH_SHORT).show()
                tcpClientRunnable.stringMsg("update")
                tcpClientRunnable.setIP(ipAddRemote)
                executor.execute(tcpClientRunnable)
                mainHandler.postDelayed(this, 2000)
            }
        }


        val udpRunnable=UdpRunnable(handler)
        executor.execute(tcpServerRunnable)



        executor.execute(udpRunnable)



        syncButton.setOnClickListener {
            // Respond to button press
            //Toast.makeText(activity,"Button Test", Toast.LENGTH_SHORT).show()

            executor.execute(tcpServerRunnable)



            executor.execute(udpRunnable)






        }
        switchOnOff.setOnClickListener{

            val switchStatus:Boolean=switchOnOff.isChecked
            if (ipAddRemote!=""){
                var jsonObj:JSONObject= JSONObject()

                jsonObj.put("powerStatus",switchStatus)
                jsonObj.put("setTempAc",sliderSetTemp.value)
                tcpClientRunnable.stringMsg(jsonObj.toString())
                tcpClientRunnable.setIP(ipAddRemote)
                executor.execute(tcpClientRunnable)
                mainHandler.postDelayed(updateTextTask,2000)
        }}
        /*
        switchOnOff.setOnCheckedChangeListener { _, isChecked ->
            val message = if (isChecked) "Switch1:ON" else "Switch1:OFF"
            val switchStatus:Boolean=isChecked
            if (ipAddRemote!=""){
                var jsonObj:JSONObject= JSONObject()

                jsonObj.put("powerStatus",switchStatus)
                jsonObj.put("setTempAc",sliderSetTemp.value)
                tcpClientRunnable.stringMsg(jsonObj.toString())
                tcpClientRunnable.setIP(ipAddRemote)
                //executor.execute(tcpClientRunnable)
                mainHandler.postDelayed(updateTextTask,2000)
            }
        }

         */




        sliderSetTemp.addOnSliderTouchListener(object : Slider.OnSliderTouchListener {
            override fun onStartTrackingTouch(slider: Slider) {
                // Responds to when slider's touch event is being started
                mainHandler.removeCallbacks(updateTextTask)
            }

            override fun onStopTrackingTouch(slider: Slider) {



                if (ipAddRemote!=""){
                    var jsonObj:JSONObject= JSONObject()
                    jsonObj.put("powerStatus",switchOnOff.isChecked)
                    jsonObj.put("setTempAc",sliderSetTemp.value)
                    tcpClientRunnable.stringMsg(jsonObj.toString())
                    tcpClientRunnable.setIP(ipAddRemote)
                    executor.execute(tcpClientRunnable)
                    mainHandler.postDelayed(updateTextTask,2000)
                }


                //executor.shutdown()




            }
        })


        textViewTempEnv.text="27C"
        textViewHumEnv.text="44%"
        return root
    }
    override fun onResume() {
        super.onResume()
        if (ipAddRemote!=""){
            mainHandler.post(updateTextTask)
        }
        //mainHandler.post(updateTextTask)
    }

    override fun onPause() {
        super.onPause()
        //executor.shutdownNow()
        Log.i("Fragment Status","Fragment is paused")
        mainHandler.removeCallbacks(updateTextTask)
    }

    override fun onStop() {
        super.onStop()

        Log.i("Fragment Status","Fragment is stopped")
    }

}




class TcpClientRunnable (handler:Handler, ip:String, port:Int): Runnable {
    var handler1=handler
    var ip=ip
    val port=port
    var msgTcp:String="Hello from the client!\n"
    fun stringMsg(msg:String){
        msgTcp=msg+"\n"
    }
    fun setIP(ipSet:String){
        ip=ipSet
    }

    override fun run() {
        // Moves the current Thread into the background

        android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_BACKGROUND)
        val message1 = Message()

        val client = Socket(ip, port)
        client.outputStream.write(msgTcp.toByteArray())

        val reader = BufferedReader(InputStreamReader(client.inputStream))
        var strLine:String
        strLine=reader.readLine()

        message1.what = STATUS_HANDLER
        //message1.obj = "Client connected :"


        message1.obj = strLine
        handler1.sendMessage(message1)



        reader.close()
        client.close()

    }

}
class TcpServerRunnable (handler:Handler): Runnable {
    var handler1=handler



    override fun run() {
        // Moves the current Thread into the background

        android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_BACKGROUND)

        //try{

            //println("Server running on port ${server.localPort}")


            //while(!isStopped){
        val server = ServerSocket(9999)
        try {

            server.soTimeout=3000
            var message = Message()
            var message1 = Message()

            val client = server.accept()
            println("Client connected : ${client.inetAddress.hostAddress}")
            message.what = 2
            message.obj = "Client connected : ${client.inetAddress.hostAddress}"
            handler1.sendMessage(message)
            val scanner = Scanner(client.inputStream)

            val reader = BufferedReader(InputStreamReader(client.inputStream))

            client.outputStream.write("Hello from the client!\n".toByteArray())
            var content: String


            //line = reader.readLine()
            //try {
            // val tokener: JSONTokener = JSONTokener(reader)
            content= reader.readText()


            //val allText = client.inputStream.bufferedReader().use(BufferedReader::readText)


            //} finally {
            reader.close()
            //}
            //server.close()
            message1.what = SYNC_HANDLER
            //message1.obj = "Client connected :"


            message1.obj = content
            handler1.sendMessage(message1)
            server.close()
        }catch (e :Exception){
            server.close()
            Log.e("Thread TCP Error",e.toString())
        }
        finally {

        }

        Log.i("Thread TCP","The tcp server is shutdown")



    }



}
class UdpRunnable (handler:Handler): Runnable {
    var handler1=handler

    override fun run() {
        // Moves the current Thread into the background

        android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_BACKGROUND)
        /*val message = Message()
        message.what = 1
        message.obj = "Test"
        handler1.sendMessage(message)
        */

        try {
            val messageStr="Hallo"
            //Open a port to send the package
            val socket = DatagramSocket()
            socket.broadcast = true
            val myTargetIP="192.168.1.255"
            val sendData = messageStr.toByteArray()
            val myTargetPort=10000
            //val myTargetPort=61142
            val sendPacket = DatagramPacket(sendData, sendData.size, InetAddress.getByName(myTargetIP), myTargetPort)
            socket.send(sendPacket)
        } catch (e: IOException) {
            //            Log.e(FragmentActivity.TAG, "IOException: " + e.message)
        }

    }

}


