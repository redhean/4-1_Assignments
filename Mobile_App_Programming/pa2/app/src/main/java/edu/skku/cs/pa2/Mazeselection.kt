package edu.skku.cs.pa2

import android.content.Context
import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.*
import com.google.gson.Gson
import com.google.gson.JsonArray
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import okhttp3.*
import java.io.IOException

class Mazeselection : AppCompatActivity() {

    class Maze(var name : String, var size : Int)

    class CustomAdapter(private val context: Context, private val items : Map<String, Int>) : BaseAdapter(){
        override fun getCount(): Int {
            //Log.i("test", items.size.toString())
            return items.size
        }

        override fun getItem(p0: Int): Any {
            return 0
        }

        override fun getItemId(p0: Int): Long {
            return 0
        }

        override fun getView(p0: Int, p1: View?, p2: ViewGroup?): View {
            val inflater: LayoutInflater = LayoutInflater.from(context)
            val view : View = inflater.inflate(R.layout.maze_entry,null)

            val name = view.findViewById<TextView>(R.id.name)
            val size = view.findViewById<TextView>(R.id.size)
            val btn = view.findViewById<Button>(R.id.start)

            val mazeEntry = items.entries.elementAt(p0)
            val name_temp = mazeEntry.key
            val size_temp = mazeEntry.value

            name.text = name_temp
            size.text = size_temp.toString()

            btn.setOnClickListener {
                val intent = Intent(context, MainMaze::class.java)
                intent.putExtra("mazeName", name_temp)
                intent.putExtra("mazeSize", size_temp)
                context.startActivity(intent)
            }

            return view
        }

    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_mazeselection)

        // Get intent
        val username = intent.getStringExtra(MainActivity.EXT_NAME)
        val name = findViewById<TextView>(R.id.username2)
        name.text= username.toString()

        // Get request
        val client = OkHttpClient()
        val gson = Gson()
        val url = "http://swui.skku.edu:1399/maps"

        val request = Request.Builder().url(url).build()
        val mazeDictionary = mutableMapOf<String, Int>()

        client.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                e.printStackTrace()
            }

            override fun onResponse(call: Call, response: Response) {
                if(!response.isSuccessful) throw IOException("Unexpected code $response")

                val responseBody = response.body!!.string()
                if(responseBody != null){
                    val mazeArray = gson.fromJson(responseBody, JsonArray::class.java)

                    for(element in mazeArray) {
                        val jsonObject = element.asJsonObject
                        val name = jsonObject.get("name").asString
                        val size = jsonObject.get("size").asInt

                        mazeDictionary[name] = size
                    }
                }
                CoroutineScope(Dispatchers.Main).launch {
                    val mazeAdapter = CustomAdapter(this@Mazeselection, mazeDictionary)
                    val mazelist = findViewById<ListView>(R.id.mazelist)
                    mazelist.adapter = mazeAdapter
                }
            }
        })
    }
}