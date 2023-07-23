package edu.skku.cs.pa2

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import android.widget.Toast
import com.google.gson.Gson
import kotlinx.coroutines.*
import okhttp3.*
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.RequestBody.Companion.toRequestBody
import java.io.IOException

class MainActivity : AppCompatActivity() {

    companion object{
        const val EXT_NAME = "username"
    }

    data class User(val username: String)
    data class ResponseData(val success: Boolean)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_signin)

        val client = OkHttpClient()
        val btn = findViewById<Button>(R.id.signin)
        val name = findViewById<EditText>(R.id.username)
        var answer: Boolean? = null

        btn.setOnClickListener {
            val user_name = name.text.toString()
            val url = "http://swui.skku.edu:1399/users"
            val user = User(user_name)
            val mediaType = "application/json".toMediaType()
            val gson = Gson()
            val requestBody = gson.toJson(user)

            val req = Request.Builder().url(url).post(requestBody.toRequestBody(mediaType)).build()

            client.newCall(req).enqueue(object : Callback{
                override fun onFailure(call: Call, e: IOException) {
                    e.printStackTrace()
                }

                override fun onResponse(call: Call, response: Response) {
                    if (!response.isSuccessful) throw IOException("Unexpected code $response")
                    val str = response.body!!.string()
                    val temp = gson.fromJson(str, ResponseData::class.java)
                    answer = temp.success

                    CoroutineScope(Dispatchers.Main).launch {
                        if (answer == true) {
                            val intent = Intent(this@MainActivity, Mazeselection::class.java).apply {
                                    putExtra(EXT_NAME, user_name)
                                }
                            startActivity(intent)
                        } else {
                            Toast.makeText(this@MainActivity, "Wrong User Name", Toast.LENGTH_SHORT).show()
                        }
                    }
                }
            })
        }
    }
}